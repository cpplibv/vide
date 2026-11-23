//

#pragma once


/// Compared by order alone, told apart by value, so a reordering of equal elements shows
struct OrderChecker {
	int order = 0;
	int value = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		ar.nvp("order", order);
		ar.nvp("value", value);
	}

	friend bool operator<(const OrderChecker& lhs, const OrderChecker& rhs) {
		return lhs.order < rhs.order;
	}

	friend bool operator>(const OrderChecker& lhs, const OrderChecker& rhs) {
		return lhs.order > rhs.order;
	}

	friend bool operator==(const OrderChecker&, const OrderChecker&) = default;
};
