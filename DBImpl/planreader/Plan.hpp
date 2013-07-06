#ifndef H_Plan_HPP
#define H_Plan_HPP

#include <memory>
#include <vector>
#include <string>

namespace plan {

enum class OperatorType { Select, Project, Sort, MergeJoin, TableScan };
enum class Cmp { EQ /*equality*/ }; // Compare operation

struct PlanOperator {
   virtual OperatorType getOperatorType() const = 0;
   virtual void print(std::ostream& out, unsigned indent=0) const = 0;
};

class BinaryOperator : public PlanOperator {
   std::unique_ptr<PlanOperator> left;
   std::unique_ptr<PlanOperator> right;
public:
   const PlanOperator& getLeft() const { return *left; };
   const PlanOperator& getRight() const { return *right; };
   void setLeft(std::unique_ptr<PlanOperator>& l) { left = std::move(l); }
   void setRight(std::unique_ptr<PlanOperator>& r) { right = std::move(r); }
};

class UnaryOperator : public PlanOperator {
   std::unique_ptr<PlanOperator> child;
public:
   const PlanOperator& getChild() const { return *child; };
   void setChild(std::unique_ptr<PlanOperator>& c) { child = std::move(c); }
};

struct LeafOperator : public PlanOperator {
};


struct Select : public UnaryOperator {
   std::vector<unsigned> attributeIds;
   Cmp cmp;
   std::vector<std::string> constants;
   OperatorType getOperatorType() const { return OperatorType::Select; }
   void print(std::ostream& out, unsigned indent=0) const;
};

struct Project : public UnaryOperator {
   std::vector<unsigned> attributeIds;
   OperatorType getOperatorType() const { return OperatorType::Project; }
   void print(std::ostream& out, unsigned indent=0) const;
};

struct Sort : public UnaryOperator {
   enum class Order { Asc, Desc };
   std::vector<unsigned> attributeIds;
   Order order;
   OperatorType getOperatorType() const { return OperatorType::Sort; }
   void print(std::ostream& out, unsigned indent=0) const;
};

struct TableScan : public LeafOperator {
   std::string name;
   OperatorType getOperatorType() const { return OperatorType::TableScan; }
   void print(std::ostream& out, unsigned indent=0) const;
};

struct MergeJoin : public BinaryOperator {
   std::vector<unsigned> attributeIdsLeft;
   Cmp cmp;
   std::vector<unsigned> attributeIdsRight;
   OperatorType getOperatorType() const { return OperatorType::MergeJoin; }
   void print(std::ostream& out, unsigned indent=0) const;
};

class Plan {
   std::unique_ptr<PlanOperator> root;
public:
   bool fromFile(const char* fileName);
   const PlanOperator& getRoot() const { return *root; };
   void print(std::ostream& out) const;
};
}
#endif
