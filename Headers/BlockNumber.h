class BlockNumber
{
public:
	BlockNumber();
	BlockNumber(ArrayIndex value);
	operator ArrayIndex() const;

	bool operator==(BlockNumber& other) const;
	bool operator==(ArrayIndex other) const;

	bool operator!=(BlockNumber& other) const;
	bool operator!=(ArrayIndex other) const;

	bool operator>=(BlockNumber& other) const;
	bool operator>=(ArrayIndex other) const;

	bool operator<=(BlockNumber& other) const;
	bool operator<=(ArrayIndex other) const;

	bool operator<(BlockNumber& other) const;
	bool operator<(ArrayIndex other) const;
	bool operator<(int other) const;

	bool operator>(BlockNumber& other) const;
	bool operator>(ArrayIndex other) const;

	BlockNumber operator++(int);
private:
	uint32_t BN;
};


