// author: Jean Gressmann
// web: www.0x42.de
// Ergänzungen von Benjamin Kaufmann:
// * Test auf void und auf incomplete type
// * this->-Qualifikation von dependent-names in SmartPointer

#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

#if defined(_MSC_VER)
#	define for if(0);else for
#	pragma warning(disable:4786)
#	pragma warning(disable:4521)
#	pragma warning(disable:4522)
#	pragma warning(disable:4355)
#	pragma warning(disable:4291)
#endif

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <cassert>

template <bool> struct StaticAssert;
template <> struct StaticAssert<true> {};
template <class T> struct is_void {enum {result = 0};};
template <> struct is_void<void> {enum {result = 1};};


// These freeResource() functions are required to handle their
// specific Null indifferently, say delete 0 is
// to do nothing.
template<class T>
struct PointerTraits_
{
	StaticAssert<!is_void<T>::result> Error_Smart_Pointer_Does_Not_Support_Void_Ptr;
	typedef T ValueType;
	typedef T& ReferenceType;
	typedef const T& ConstReferenceType;
	typedef T* PointerType;
	typedef const T* ConstPointerType;
	static const PointerType Null;

	static void freeResource(PointerType p)
	{
		enum { no_incomplete_types_please = sizeof(*p) };
		delete p;
	}
};

template<class T>
const typename PointerTraits_<T>::PointerType PointerTraits_<T>::Null = 0;


template<class T>
struct ArrayPointerTraits_
{
	typedef T ValueType;
	typedef T& ReferenceType;
	typedef const T& ConstReferenceType;
	typedef T* PointerType;
	typedef const T* ConstPointerType;
	static const PointerType Null;

	static void freeResource(PointerType p)
	{
		delete [] p;
	}
};

template<class T>
const typename ArrayPointerTraits_<T>::PointerType ArrayPointerTraits_<T>::Null = 0;

template<class Traits>
class SingleOwner_
{
public:
	typedef typename Traits::PointerType PT;
	typedef typename Traits::ConstPointerType CPT;
	static CPT null() { return Traits::Null; }
private:
	PT managed_;

	SingleOwner_(const SingleOwner_&);
	SingleOwner_& operator=(const SingleOwner_&);
public:
	~SingleOwner_()
	{
		Traits::freeResource(managed_);
	}
	explicit
	SingleOwner_()
		:	managed_(Traits::Null)
	{}
	explicit
	SingleOwner_(PT p)
		:	managed_(p)
	{}

	SingleOwner_(SingleOwner_& o)
		:	managed_(Traits::Null)
	{
		swap(o);
	}

	SingleOwner_& operator=(SingleOwner_& o)
	{
		swap(o);
		return *this;
	}
	SingleOwner_& operator=(PT p)
	{
		SingleOwner_().swap(*this);
		managed_ = p;
		return *this;
	}
	bool unique() const { return true; }
	size_t count() const { return 1; }
	PT get() { return managed_; }
	CPT get() const { return managed_; }
	void swap(SingleOwner_& o) // nothrow guarantee
	{
		std::swap(managed_, o.managed_);
	}
	void reset()
	{
		SingleOwner_().swap(*this);
	}
	void release()
	{
		managed_ = Traits::Null;
	}
};

template<class Traits>
class ReferenceCountedOwner_
{
public:
	typedef typename Traits::PointerType PT;
	typedef typename Traits::ConstPointerType CPT;
	static CPT null() { return Traits::Null; }
private:
	class RefCount_
	{
		PT managed_;
		size_t count_;
		RefCount_();
		RefCount_(const RefCount_&);
		RefCount_& operator=(const RefCount_&);
	public:
		explicit
		RefCount_(PT p)
			:	managed_(p)
			,	count_(1)
		{}
		void inc()
		{
			++count_;
		}
		size_t count() const { return count_; }
		bool unique() const { return count_ == 1; }
		void dec()
		{
			--count_;
			if(!count_)
			{
				PT deleteme = managed_;
				delete this;
				Traits::freeResource(deleteme);
			}

		}
		void weakDec()
		{
			--count_;
			if(!count_)
			{
				delete this;
			}
		}
		PT get() { return managed_;	}
		CPT get() const { return managed_; }
	};
	RefCount_* refCount_;

	void decIfValidRefCount()
	{
		if(refCount_)
		{
			RefCount_* temp = 0;
			std::swap(temp, refCount_);
			temp->dec();
		}

	}
	void incIfValidRefCount()  // nothrow guarantee
	{
		if(refCount_)
			refCount_->inc();
	}
	static RefCount_* createRefCount(PT p)
	{
		return p == Traits::Null ? 0 : new RefCount_(p);
	}
public:
	~ReferenceCountedOwner_()
	{
		decIfValidRefCount();
	}
	explicit
	ReferenceCountedOwner_()
		:	refCount_(0)
	{}
	ReferenceCountedOwner_(PT p)
		:	refCount_(createRefCount(p))
	{}
	ReferenceCountedOwner_(const ReferenceCountedOwner_& o)
		:	refCount_(o.refCount_)
	{
		incIfValidRefCount();
	}
	ReferenceCountedOwner_& operator=(const ReferenceCountedOwner_& o)
	{
		ReferenceCountedOwner_().swap(*this);

		refCount_ = o.refCount_;
		incIfValidRefCount();

		return *this;
	}
	ReferenceCountedOwner_& operator=(PT p)
	{
		ReferenceCountedOwner_().swap(*this);
		refCount_ = createRefCount(p);

		return *this;
	}
	bool unique() const { return refCount_ ? refCount_->unique() : true; }  // nothrow guarantee
	size_t count() const { return refCount_ ? refCount_->count() : 1; }  // nothrow guarantee
	PT get() { return refCount_ ? refCount_->get() : Traits::Null; }
	CPT get() const { return refCount_ ? refCount_->get() : Traits::Null; }
	void swap(ReferenceCountedOwner_& o)  // nothrow guarantee
	{
		std::swap(refCount_, o.refCount_);
	}
	void reset()
	{
		ReferenceCountedOwner_().swap(*this);
	}
	void release()
	{
		if(refCount_)
			refCount_->weakDec();
		refCount_ = 0;
	}
};

template
<
	class T,
	class Traits = PointerTraits_<T>,
	class RefCountPolicy = SingleOwner_<Traits>
>
class SmartPointer : public RefCountPolicy
{
public:
	typedef typename Traits::ValueType VT;
	typedef typename Traits::PointerType PT;
	typedef typename Traits::ConstPointerType CPT;
	typedef typename Traits::ReferenceType RT;
	typedef typename Traits::ConstReferenceType CRT;

	explicit
	SmartPointer()
	{}
	explicit
	SmartPointer(PT p)
		:	RefCountPolicy(p)
	{}

	SmartPointer(const SmartPointer& o)
		:	RefCountPolicy(o)
	{}
	SmartPointer(SmartPointer& o)
		:	RefCountPolicy(o)
	{}
	SmartPointer& operator=(SmartPointer& o)
	{
		RefCountPolicy::operator=(o);
		return *this;
	}
	SmartPointer& operator=(const SmartPointer& o)
	{
		RefCountPolicy::operator=(o);
		return *this;
	}
	SmartPointer& operator=(PT p)
	{
		RefCountPolicy::operator=(p);
		return *this;
	}
	bool operator==(const SmartPointer& o) const
	{
		return this->get() == o.get();
	}
	bool operator!=(const SmartPointer& o) const
	{
		return this->get() != o.get();
	}

	operator bool() const { return this->get() != Traits::Null; }
	PT operator->() { return this->get(); }
	CPT operator->() const { return this->get(); }
	RT operator*() { return *this->get(); }
	CRT operator*() const { return *this->get(); }
};

template<class T, class Traits = PointerTraits_<T> >
class SharedPtr : public SmartPointer<T, Traits, ReferenceCountedOwner_<Traits> >
{
	typedef SmartPointer<T, Traits, ReferenceCountedOwner_<Traits> > Base;
public:
	typedef typename Base::PT PT;
	typedef typename Base::VT VT;
	explicit
	SharedPtr()
		:	Base()
	{}

	SharedPtr(PT p)
		:	Base(p)
	{}
	SharedPtr(const SharedPtr& o)
		:	Base(o)
	{}
	SharedPtr& operator=(PT p) {
		Base::operator=(p);
		return *this;
	}
};


#endif
