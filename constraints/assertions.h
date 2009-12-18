#ifndef _ASSERTIONS_H
#define	_ASSERTIONS_H
#include <stack>
#include "ioperator.h"
#include "logicaloperator.h"
#include "constraint.h"
#include "notoperator.h"
#include "equaltoconstraint.h"
#include "operatorstack.h"
#include "expression.h"
#include "is.h"

class Assert
{
public:
  template <typename T>
  static void That(T actual, Expression& constraint)
  {
     std::stack<bool> operatorResultsStack;
    std::stack<std::string> expectations;

    IOperator* op;
    while ((op = constraint.PopOperator()) != NULL)
    {
      PerformOperation(op, actual, operatorResultsStack, expectations);
    }

    if (!operatorResultsStack.top())
    {
      throw AssertionException(CreateErrorText(expectations, actual));
    }
  }

  static void That(const char* actual, Expression& constraint)
  {
	  return That<std::string>(std::string(actual), constraint);
  }

  static void That(bool& actual)
  {
    if (!actual)
    {
      throw AssertionException("Expected condition to be true");
    }
  }

private:

  template <typename T>
  static void PerformOperation(IOperator * op, T actual, std::stack<bool>& resultStack, std::stack<std::string>& expectation)
  {
    expectation.push(op->ExpectationText());

    if (op->IsLogicalOperator())
    {
      LogicalOperator* logical = dynamic_cast<LogicalOperator*> (op);
      logical->Evaluate(resultStack);
    }
    else
    {
      Constraint<T>* constraint = dynamic_cast<Constraint<T>*> (op);
      resultStack.push(constraint->Evaluate(actual));
    }

    delete op;
  }

  template <typename T>
  static std::string CreateErrorText(std::stack<std::string>& operatorExpectations, T actual)
  {
     std::ostringstream str;
    str << "Expected: ";

    while (!operatorExpectations.empty())
    {
      str << operatorExpectations.top();
      operatorExpectations.pop();

      if (!operatorExpectations.empty())
      {
        str << " ";
      }
    }
    str << std::endl;

    str << "Actual: " << actual << std::endl;

    return str.str();
  }
};

#endif	/* _ASSERTIONS_H */

