template<typename E>
class ItrBase {
public:
	ItrBase() {}
	virtual ~ItrBase() {}
	virtual void  operator++() {}
	virtual E&    operator*() const { return E(); }
	virtual ItrBase* clone() const { return new ItrBase(*this); }
	// The == operator is non-virtual. It checks that the
	// derived objects have compatible types, then calls the
	// virtual comparison function equal.
	bool operator==(const ItrBase& o) const {
		return typeid(*this) == typeid(o) && equal(o);
	}
protected:
	virtual bool equal(const ItrBase& o) const { return true; }
};

template<typename E>
class Itr {
public:
	Itr() : itr_(0) {}
	~Itr() { delete itr_; }
	Itr(const Itr& o) : itr_(o.itr_->clone()) {}
	Itr& operator=(const Itr& o) {
		if (itr_ != o.itr_) { delete itr_; itr_ = o.itr_->clone(); }
		return *this;
	}
	Itr&  operator++() { ++(*itr_); return *this; }
	E&    operator*() const { return *(*itr_); }
	bool  operator==(const Itr& o) const {
		return (itr_ == o.itr_) || (*itr_ == *o.itr_);
	}
	bool  operator!=(const Itr& o) const { return !(*this == o); }

protected:
	ItrBase<E>* itr_;
};
