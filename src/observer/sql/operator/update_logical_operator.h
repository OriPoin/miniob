#pragma once

#include <vector>

#include "sql/operator/logical_operator.h"
#include "sql/parser/parse_defs.h"

/**
 * @brief 逻辑算子，更新
 * @ingroup LogicalOperator
 */
class UpdateLogicalOperator : public LogicalOperator
{
public:
  UpdateLogicalOperator(Table *table, std::vector<SetVariableSqlNode> *assignments);
  ~UpdateLogicalOperator() override = default;

  [[nodiscard]] LogicalOperatorType type() const override { return LogicalOperatorType::UPDATE; }

  [[nodiscard]] Table                                 *table() const { return table_; }
  [[nodiscard]] const std::vector<SetVariableSqlNode> &assignments() const { return *assignments_; }
  std::vector<SetVariableSqlNode>                     &assignments() { return *assignments_; }

private:
  Table                           *table_       = nullptr;
  std::vector<SetVariableSqlNode> *assignments_ = nullptr;
};