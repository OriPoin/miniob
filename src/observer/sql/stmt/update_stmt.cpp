/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#include "common/log/log.h"
#include "storage/db/db.h"
#include "sql/stmt/update_stmt.h"
#include "sql/stmt/filter_stmt.h"

UpdateStmt::UpdateStmt(Table *table, std::vector<SetVariableSqlNode> *assignments, FilterStmt *filter_stmt)
    : table_(table), assignments_(assignments), filter_stmt_(filter_stmt)
{}

RC UpdateStmt::create(Db *db, UpdateSqlNode &update_sql, Stmt *&stmt)
{
  RC rc = RC::SUCCESS;

  const char *table_name = update_sql.relation_name.c_str();
  if (nullptr == db || nullptr == table_name || update_sql.assignments.empty()) {
    LOG_WARN("invalid argument. db=%p, relation_name=%p, value_num=%d",
        db, table_name, static_cast<int>(update_sql.assignments.size()));
    return RC::INVALID_ARGUMENT;
  }
  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));
  // check whether field match
  rc                                              = RC::SCHEMA_FIELD_NOT_EXIST;
  const TableMeta              &table_meta        = table->table_meta();
  const std::vector<FieldMeta> *table_fields_meta = table_meta.field_metas();
  int                           field_meta_begin  = table_meta.sys_field_num();
  int                           field_meta_end    = table_meta.field_num();
  for (auto &assignment : update_sql.assignments) {
    for (int j = field_meta_begin; j < field_meta_end; j++) {
      const char *table_field_name = table_fields_meta->at(j).name();
      if (std::string(table_field_name) == assignment.name) {
        const AttrType field_type = table_fields_meta->at(j).type();
        const AttrType value_type = assignment.value.attr_type();
        if (field_type != value_type) {
          if (RC::SUCCESS != Value::type_cast(assignment.value, field_type, value_type)) {
            LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
                                          table_name, table_field_name, field_type, value_type);
            return RC::SCHEMA_FIELD_TYPE_MISMATCH;
          }
        }
        rc = RC::SUCCESS;
        break;
      }
    }
    if (rc != RC::SUCCESS) {
      LOG_WARN("update statement field not exist. field name=%s", assignment.name.c_str());
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
  }
  // set filters
  FilterStmt *filter_stmt = nullptr;
  rc                      = FilterStmt::create(
      db, table, &table_map, update_sql.conditions.data(), static_cast<int>(update_sql.conditions.size()), filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    return rc;
  }
  stmt = new UpdateStmt(table, &update_sql.assignments, filter_stmt);
  return rc;
}
