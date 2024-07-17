#pragma once

#include <string>
#include <utility>

#include "sql/stmt/stmt.h"

class Db;

/**
 * @brief 表示删除表的语句
 * @ingroup Statement
 * @details 删除表
 */
class DropTableStmt : public Stmt
{
public:
  explicit DropTableStmt(std::string table_name) : table_name_(std::move(table_name)) {}
  ~DropTableStmt() override = default;

  [[nodiscard]] StmtType type() const override { return StmtType::DROP_TABLE; }

  [[nodiscard]] const std::string &table_name() const { return table_name_; }

  static RC create(Db *db, const DropTableSqlNode &drop_table, Stmt *&stmt);

private:
  std::string table_name_;
};