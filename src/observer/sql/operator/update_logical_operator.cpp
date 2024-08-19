#include "sql/operator/update_logical_operator.h"

UpdateLogicalOperator::UpdateLogicalOperator(Table *table, std::vector<SetVariableSqlNode> *assignments)
    : table_(table), assignments_(assignments)
{}
