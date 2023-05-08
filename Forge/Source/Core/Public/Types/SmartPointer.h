#ifndef SMART_POINTER_H
#define SMART_POINTER_H

#include <Core/Public/Debug/Debug.h>
#include <Core/Public/Common/Common.h>
#include <Core/Public/Types/TDelegate.h>
#include <Core/Public/Types/RefCounter.h>
#include <Core/Public/Algorithm/GeneralUtilities.h>

using namespace Forge::Algorithm;

namespace Forge {
	namespace Common
	{
		template <typename InType>
		class TSharedPtr;

		template<typename InType>
		class TWeakPtr;

		/**
		 * @brief Manages the storage of raw pointers, provides garbage-collection,
		 * and allows multiple TSharedPtr to share management of the same pointer.
		 * This ultimatly prevents accidental memory leaks and helps in monitoring
		 * shared memory locations.
		 * 
		 * Objects of TSharedPtr have the ability of taking ownership of a pointer
		 * and share that ownership. Upon taking ownership, a reference counter 
		 * is incremented indicating the number of TSharedPtr objects managing the
		 * pointer. 
		 * 
		 * When a TSharedPtr is destroyed the reference counter is decremented.
		 * A TSharedPtr becomes responsible for the deletion of the stored pointer
		 * when it is the last object to release ownership.
		 * 
		 * @author Karim Hisham
		 */
		template<typename InType>
		class TSharedPtr final
		{
		public:
			using Type         = InType;
			using TypePtr      = InType*;
			using TypeRef      = InType&;
			using TypeMoveRef  = InType&&;
			using ConstType    = const InType;
			using ConstTypeRef = const InType&;
 			using ConstTypePtr = const InType*;

		private:
			using SelfType         = TSharedPtr<Type>;
			using SelfTypePtr      = TSharedPtr<Type>*;
			using SelfTypeRef      = TSharedPtr<Type>&;
			using SelfTypeMoveRef  = TSharedPtr<Type>&&;
			using ConstSelfType    = const TSharedPtr<Type>;
			using ConstSelfTypeRef = const TSharedPtr<Type>&;
			using ConstSelfTypePtr = const TSharedPtr<Type>*;

		private:
			TypePtr m_raw_ptr;

		private:
			RefCounterPtr m_ref_counter;

		private:
			TDelegate<Void(TypePtr)> m_deleter_callback;

		private:
			Void InternalDecrementRef(Void)
			{
				if (m_ref_counter)
				{
					m_ref_counter->DecrementRef();

					if (!m_ref_counter->GetRefCount())
					{
						m_deleter_callback.Invoke(m_raw_ptr);

						FORGE_SAFE_DELETE_UNIT(m_ref_counter)
					}
				}
			}

		public:
			/**
			 * @brief Default constructor.
			 */
			TSharedPtr(Void)
				: m_raw_ptr(nullptr), m_ref_counter(nullptr)
			{
				m_deleter_callback = [](TypePtr ptr) -> Void
				{
					FORGE_SAFE_DELETE_UNIT(ptr)
				};
			}

			/**
			 * @brief Pointer constructor.
			 */
			TSharedPtr(TypePtr ptr)
				: m_raw_ptr(ptr), m_ref_counter(new RefCounter())
			{
				m_ref_counter->IncrementRef();

				m_deleter_callback = [](TypePtr ptr) -> Void
				{
					FORGE_SAFE_DELETE_UNIT(ptr)
				};
			}
			
			/**
			 * @brief Pointer and Deleter constructor.
			 */
			TSharedPtr(TypePtr ptr, TDelegate<Void(TypePtr)> deleter)
				: m_raw_ptr(ptr), m_ref_counter(new RefCounter()), m_deleter_callback(deleter) 
			{
				m_ref_counter->IncrementRef();
			}
		   
		public:
			/**
			 * @brief Shared pointer move constructor.
			 */
			TSharedPtr(SelfTypeMoveRef other)
			{
				*this = Move(other);
			}

			/**
			 * @brief Shared pointer copy constructor.
			 */
			TSharedPtr(ConstSelfTypeRef other)
			{
				*this = other;
			}

			/**
			 * @brief Weak pointer move constructor.
			 */
			TSharedPtr(TWeakPtr<Type>&& other)
			{
				*this = Move(other);
			}

			/**
			 * @brief Weak pointer copy constructor.
			 */
			TSharedPtr(const TWeakPtr<Type>& other)
			{
				*this = other;
			}
			
		public:
			/**
			 * @brief Default constructor.
			 */
			~TSharedPtr(Void)
			{
				InternalDecrementRef();
			}

		public:
			/**
			 * @brief Shared pointer move assigment.
			 */
			SelfTypeRef operator =(SelfTypeMoveRef other)
			{
				if (m_raw_ptr == other.m_raw_ptr)
					return *this;

				InternalDecrementRef();

				m_raw_ptr = other.m_raw_ptr;
				m_ref_counter = other.m_ref_counter;

				if (m_ref_counter)
					m_ref_counter->IncrementRef();

				return *this;
			}
			
			/**
			 * @brief Shared pointer copy assigment.
			 */
			SelfTypeRef operator =(ConstSelfTypeRef other)
			{
				if (m_raw_ptr == other.m_raw_ptr)
					return *this;

				InternalDecrementRef();

				m_raw_ptr = other.m_raw_ptr;
				m_ref_counter = other.m_ref_counter;

				if (m_ref_counter)
					m_ref_counter->IncrementRef();

				return *this;
			}

			/**
			 * @brief Weak pointer move assigment.
			 */
			SelfTypeRef operator =(TWeakPtr<Type>&& other)
			{
				if (m_raw_ptr == other.m_raw_ptr)
					return *this;

				InternalDecrementRef();

				m_raw_ptr = other.m_raw_ptr;
				m_ref_counter = other.m_ref_counter;

				if (m_ref_counter)
					m_ref_counter->IncrementRef();

				return *this;
			}

			/**
			 * @brief Weak pointer copy assigment.
			 */
			SelfTypeRef operator =(const TWeakPtr<Type>& other)
			{
				if (m_raw_ptr == other.m_raw_ptr)
					return *this;

				InternalDecrementRef();

				m_raw_ptr = other.m_raw_ptr;
				m_ref_counter = other.m_ref_counter;

				if (m_ref_counter)
					m_ref_counter->IncrementRef();

				return *this;
			}

		public:
			/**
			 * @brief Equality operator.
			 */
			Bool operator ==(ConstSelfTypeRef other) const
			{
				return m_raw_ptr == other.m_raw_ptr;
			}

			/**
			 * @brief Inequality operator.
			 */
			Bool operator !=(ConstSelfTypeRef other) const
			{
				return m_raw_ptr != other.m_raw_ptr;
			}

		public:
			/**
			 * @brief Dereference operator.
			 */
			TypeRef operator *()
			{
				if (!m_raw_ptr)
					FORGE_EXCEPT(ExceptionType::FORGE_INVALID_OPERATION, "Shared pointer is storing a null pointer")

				return *m_raw_ptr;
			}

		public:
			/**
			 * @brief Arrow operator.
			 */
			TypePtr operator ->()
			{
				if (!m_raw_ptr)
					FORGE_EXCEPT(ExceptionType::FORGE_INVALID_OPERATION, "Shared pointer is storing a null pointer")

				return m_raw_ptr;
			}
		
		public:
			/**
			 * @brief Checks whether the stored pointer is a null pointer.
			 *
			 * @returns True if the stored pointer is null, otherwise false.
			 */
			Bool IsNull(Void) const
			{
				if (!m_raw_ptr)
					return FORGE_TRUE;

				return FORGE_FALSE;
			}

			/**
			 * @brief Checks whether the shared pointer does not share ownership
			 * over its pointer with other shared pointers.
			 *
			 * @returns True if the TShartedPtr object is unique, otherwise false.
			 * 
			 * @throws InvalidOperationException if the reference-counter is null.
			 */
			Bool IsUnique(Void) const
			{
				if (!m_ref_counter)
					FORGE_EXCEPT(ExceptionType::FORGE_INVALID_OPERATION)

				return FORGE_TRUE ? m_ref_counter->GetRefCount() == 1 : FORGE_FALSE;
			}

		public:
			/**
			 * @brief Gets the number of TSharedPtr objects that share ownership
			 * over the same stored pointer including itself.
			 * 
			 * @returns ConstSize storing the number of refrences to the stored
			 * pointer.
			 * 
			 * @throws InvalidOperationException if the reference-counter is null.
			 */
			ConstSize GetRefCount(Void) const
			{
				if (!m_ref_counter)
					FORGE_EXCEPT(ExceptionType::FORGE_INVALID_OPERATION)
				
				return m_ref_counter->GetRefCount();
			}

			/**
			 * @brief Gets the stored pointer owned by the TSharedPtr object. 
			 * 
			 * Avoid manually deleting the stored pointer as this may cause
			 * undefined behaviour. 
			 * 
			 * @returns TypePtr storing the address of the stored pointer.
			 * 
			 * @throws InvalidOperationException if the reference-counter is null.
			 */
			TypePtr GetRawPtr(Void)
			{
				if (!m_ref_counter)
					FORGE_EXCEPT(ExceptionType::FORGE_INVALID_OPERATION)

				return m_ref_counter;
			}

		public:
			/**
			 * @brief Swaps the content of the TSharedPtr object with the other,
			 * without altering the reference count of either or destroying them.
			 * 
			 * @param[in] other The TSharedPtr object to swap content with.
			 */
			Void Swap(SelfTypeRef other)
			{
				Algorithm::Swap(*this, other);
			}

			/**
			 * @brief Resets the raw pointer to null by default, or sets a new ownership of 
			 * the pointer passed and releases ownership of the old pointer.
			 * 
			 * @param[in] ptr The new pointer that will be owned by the TSharedPtr object.
			 */
			Void Reset(TypePtr ptr = nullptr)
			{
				InternalDecrementRef();

				if (ptr)
				{
					m_ref_counter = new RefCounter();
					m_ref_counter->IncrementRef();
				}

				m_raw_ptr = ptr;
			}
		};

		FORGE_TYPEDEF_TEMPLATE_DECL(TSharedPtr)

		/**
		 * @brief Manages the storage of raw pointers, by holding non-owning 
		 * "weak" references to data managed by TSharedPtr. Unlike TSharedPtr it 
		 * does not provide garabge-collection. 
		 * 
		 * TWeakPtr models temporary ownership, allowing data to be accessed only 
		 * if it exists, and may be deleted at any time. 
		 * 
		 * TWeakPtr is used to track the object, and is converted to TSharedPtr 
		 * to assume temporary ownership and access the data. If the original 
		 * TSharedPtr object is destroyed at this time, the object's lifetime 
		 * is extended until the temporary TSharedPtr is destroyed as well.
		 * 
		 * @author Karim Hisham
		 */
		template<typename InType>
		class TWeakPtr final
		{
		public:
			using Type         = InType;
			using TypePtr      = InType*;
			using TypeRef      = InType&;
			using TypeMoveRef  = InType&&;
			using ConstType    = const InType;
			using ConstTypeRef = const InType&;
			using ConstTypePtr = const InType*;

		private:
			using SelfType         = TWeakPtr<Type>;
			using SelfTypePtr      = TWeakPtr<Type>*;
			using SelfTypeRef      = TWeakPtr<Type>&;
			using SelfTypeMoveRef  = TWeakPtr<Type>&&;
			using ConstSelfType    = const TWeakPtr<Type>;
			using ConstSelfTypeRef = const TWeakPtr<Type>&;
			using ConstSelfTypePtr = const TWeakPtr<Type>*;

		private:
			TypePtr m_raw_ptr;

		private:
			RefCounterPtr m_ref_counter;

		private:
			Void InternalDecrementRef(Void)
			{
				if (m_ref_counter)
					m_ref_counter->DecrementWeakRef();
			}

		public:
			/**
			 * @brief Default constructor.
			 */
			TWeakPtr(Void)
				: m_raw_ptr(nullptr), m_ref_counter(nullptr) {}

		public:
			/**
			 * @brief Weak pointer move constructor.
			 */
			TWeakPtr(SelfTypeMoveRef other)
			{
				*this = Move(other);
			}

			/**
			 * @brief Weak pointer copy constructor.
			 */
			TWeakPtr(ConstSelfTypeRef other)
			{
				*this = other;
			}

			/**
			 * @brief Smart pointer move constructor.
			 */
			TWeakPtr(TSharedPtr<Type>&& other)
			{
				*this = Move(other);
			}

			/**
			 * @brief Smart pointer move constructor.
			 */
			TWeakPtr(const TSharedPtr<Type>& other)
			{
				*this = other;
			}

		public:
			/**
			 * @brief Default destructor.
			 */
			~TWeakPtr(Void)
			{
				InternalDecrementRef();
			}

		public:
			SelfTypeRef operator =(SelfTypeMoveRef other)
			{
				if (m_raw_ptr == other.m_raw_ptr)
					return *this;

				InternalDecrementRef();

				m_raw_ptr = other.m_raw_ptr;
				m_ref_counter = other.m_ref_counter;

				if (m_ref_counter)
					m_ref_counter->IncrementWeakRef();

				return *this;
			}

			SelfTypeRef operator =(ConstSelfTypeRef other)
			{
				if (m_raw_ptr == other.m_raw_ptr)
					return *this;

				InternalDecrementRef();

				m_raw_ptr = other.m_raw_ptr;
				m_ref_counter = other.m_ref_counter;

				if (m_ref_counter)
					m_ref_counter->IncrementWeakRef();

				return *this;
			}

			SelfTypeRef operator =(TSharedPtr<Type>&& other)
			{
				if (m_raw_ptr == other.m_raw_ptr)
					return *this;

				InternalDecrementRef();

				m_raw_ptr = other.m_raw_ptr;
				m_ref_counter = other.m_ref_counter;

				if (m_ref_counter)
					m_ref_counter->IncrementWeakRef();

				return *this;
			}

			SelfTypeRef operator =(const TSharedPtr<Type>& other)
			{
				if (m_raw_ptr == other.m_raw_ptr)
					return *this;

				InternalDecrementRef();

				m_raw_ptr = other.m_raw_ptr;
				m_ref_counter = other.m_ref_counter;

				if(m_ref_counter)
					m_ref_counter->IncrementWeakRef();

				return *this;
			}

		public:
			/**
			 * @brief Equality operator.
			 */
			Bool operator ==(ConstSelfTypeRef other) const
			{
				return m_raw_ptr == other.m_raw_ptr;
			}

			/**
			 * @brief Inequality operator.
			 */
			Bool operator !=(ConstSelfTypeRef other) const
			{
				return m_raw_ptr != other.m_raw_ptr;
			}

		public:
			/**
			 * @brief Checks whether the TWeakPtr object is either empty or there
			 * are no more TSharedPtr referencing the stored pointer.
			 * 
			 * @return True if no more TSharedPtr are referencing the stored 
			 * pointer and has been deallocated or the TWeakPtr object is empty.
			 * 
			 * @throws InvalidOperationException if the reference-counter is null.m_ref_counter->
			 */
			Bool IsExpired(Void)
			{
				if (!m_ref_counter)
					FORGE_EXCEPT(ExceptionType::FORGE_INVALID_OPERATION)

				return !m_raw_ptr || m_ref_counter->GetRefCount() == 0;
			}

			/**
			 * @brief Checks whether the stored pointer is not a null pointer.
			 *
			 * @return True if the stored pointer is not null.
			 */
			Bool IsNull(Void)
			{
				if (!m_raw_ptr)
					return FORGE_TRUE;

				return FORGE_FALSE;
			}

		public:
			/**
			 * @brief Gets the number of TSharedPtr objects that share ownership
			 * over the same stored pointer. TWeakPtr objects are not counted.
			 *
			 * @return Number of refrences to the stored pointer.
			 * 
			 * @throws InvalidOperationException if the stored pointer is null.
			 */
			ConstSize GetRefCount(Void)
			{
				if (!m_ref_counter)
					FORGE_EXCEPT(ExceptionType::FORGE_INVALID_OPERATION)

				return m_ref_counter->GetRefCount();
			}

		public:
			/**
			 * @brief Locks the stored pointer and prevents it from being released.
			 * 
			 * @return TSharedPtr with the information preserved by the TWeakPtr
			 * object if it is not expired. If the TWeakPtr object has expired, 
			 * the function returns an empty TSharedPtr.
			 */
			TSharedPtr<Type> Lock(Void)
			{
				if (IsExpired())
					return TSharedPtr<Type>();
				
				return TSharedPtr<Type>(m_raw_ptr);
			}

		public:
			/**
			 * @brief Exchanges the contents of the TWeakPtr object with the other,
			 * without altering the weak reference count of either or destroying them.
			 *
			 * @param[in] other The TWeakPtr object to swap content with.
			 */
			Void Swap(SelfTypeRef other)
			{
				Algorithm::Swap(*this, other);
			}

			/**
			 * @brief Resets the TWeakPtr and makes it empty.
			 */
			Void Reset(Void)
			{
				m_raw_ptr = nullptr;

				InternalDecrementRef();
			}
		};

		FORGE_TYPEDEF_TEMPLATE_DECL(TWeakPtr)
	}
}

#endif // SMART_POINTER_H
