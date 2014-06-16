#include "mysql_oa_table.h"
#include "logger.h"
#include "protocol_id_list.h"
#include "function.h"

mysql_oa_table::mysql_oa_table(void)
{
}


mysql_oa_table::~mysql_oa_table(void)
{
}

DB_RESULT mysql_oa_table::query_sql(const std::string& strsql, uint64_t& urows, std::string& errcode)
{
    urows = 0;
    errcode.clear();
    if (true==strsql.empty())
    {
        errcode = "sql error";
        return DR_FAIL;
    }

    INIT_MYSQL_OA_CONNECTION();

    mysqlpp::SimpleResult res = query.execute();
    if (false == res)
    {
        errcode = query.error();
        LLOG(ERR, "query_sql(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    urows = res.rows();

    return res.rows() > 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::get_user_verify_info(const std::string& userid, user_verify_info& info)
{
    INIT_MYSQL_OA_CONNECTION();

    query << "SELECT emp.employee_password,emp.is_delete,emp.employee_name,age.agency_name,post.post_name,post.post_privileges,emp.skill_id,emp.demo_account "
         << "FROM oa_employee emp,oa_post post,oa_agency age "
         << "WHERE emp.employee_id='" << userid.c_str() << "' "
         << "AND emp.post_id=post.post_id "
         << "AND emp.agency_id=age.agency_id "
         << "LIMIT 0,1;";

    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        LLOG(ERR, "verify_user(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    mysqlpp::Row row;
    while (row = use_res.fetch_row())
    {
        info.employee_password = row["employee_password"].empty() ? "" : row["employee_password"].c_str();
        info.employee_name     = row["employee_name"].empty() ? "" : row["employee_name"].c_str();
        info.agency_name       = row["agency_name"].empty() ? "" : row["agency_name"].c_str();
        info.post_name         = row["post_name"].empty() ? "" : row["post_name"].c_str();
        info.post_privileges    = row["post_privileges"].empty() ? "" : row["post_privileges"].c_str();
        info.is_delete         = row["is_delete"].empty() ? 0 : atoi(row["is_delete"].c_str());
        info.skill_id          = row["skill_id"].empty() ? 0 : atoi(row["skill_id"].c_str());
        info.demo_account      = row["demo_account"].empty() ? "" : row["demo_account"].c_str();        
        return DR_SUCCEED;
    }
    return DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_post_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_client_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        OA_POST item;
        item.set_post_id(row["post_id"].empty() ? 0 : atoi(row["post_id"]));
        item.set_post_name(row["post_name"].empty() ? "" : row["post_name"]);
        item.set_post_privileges(row["post_privileges"].empty() ? "" : row["post_privileges"]);
        item.set_privileges_remark(row["privileges_remark"].empty() ? "" : row["privileges_remark"]);
        item.set_post_remark(row["post_remark"].empty() ? "" : row["post_remark"]);

        records.add_records(item.SerializeAsString());
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_agency_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_AGENCY item;
    SQL_RECORDS records;
    mysqlpp::Row row;
    records.set_record_type(proto.record_type());
    records.set_record_count(0);
    records.set_record_begin(0);
    records.set_record_end(0);

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_agency_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_agency_id(row["agency_id"].empty() ? 0 : atoi(row["agency_id"].c_str()));
        item.set_agency_name(row["agency_name"].empty() ? "" : row["agency_name"]);
        item.set_owner_id(row["owner_id"].empty() ? 0 : _atoi64(row["owner_id"].c_str()));
        item.set_owner_name(row["owner_name"].empty() ? "" : row["owner_name"]);
        item.set_agency_id_prev(row["agency_id_prev"].empty() ? 0 : atoi(row["agency_id_prev"].c_str()));
        item.set_remark(row["remark"].empty() ? "" : row["remark"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }
    records.set_record_count(records.records_size());
    records.set_record_begin(0);
    records.set_record_end(records.records_size()-1);

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_insert_update_delete(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    SQL_RECORDS records;
    records.set_record_type(proto.record_type());
    records.set_record_count(0);
    records.set_record_begin(0);
    records.set_record_end(0);

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::SimpleResult res = query.execute();
    if (false == res)
    {
        proto_result.set_result_code(std::string(query.error()));
        proto_result.set_result_data(records.SerializeAsString());

        LLOG(ERR, "request_insert_update_delete(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    if (res.insert_id() > 0)
    {
        char id[16] = {0};
        sprintf_s(id, "%I64u", res.insert_id());
        records.set_retention(id);
    }

    records.set_record_count(res.rows());
    proto_result.set_result_data(records.SerializeAsString());

    return res.rows() > 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_client_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_CLIENT_VIEWS item;
    SQL_RECORDS records;
    mysqlpp::Row row;
    
    records.set_record_type(proto.record_type());
    records.set_record_count(0);
    do 
    {
        if (RECORD_CLIENT_COUNT != proto.record_type())
        {
            break;
        }
        if (false == proto.has_retention() ||
            true == proto.retention().empty())
        {
            break;
        }
        INIT_MYSQL_OA_CONNECTION();
        query << proto.retention();
        mysqlpp::UseQueryResult use_res = query.use();
        if (false == use_res)
        {
            LLOG(ERR, "request_client_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
            break;
        }
        while (row = use_res.fetch_row())
        {
            std::string strcount = row[0].c_str();
            records.set_record_count(_atoi64(strcount.c_str()));
            break;
        }
    } while (0);

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_client_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_client_id(row["client_id"].empty() ? "" : row["client_id"]);
        item.set_client_mobile(row["client_mobile"].empty() ? "" : row["client_mobile"]);
        item.set_client_phone(row["client_phone"].empty() ? "" : row["client_phone"]);
        item.set_client_over_amt(row["client_over_amt"].empty() ? 0.00 : atof(row["client_over_amt"].c_str()));
        item.set_client_name(row["client_name"].empty() ? "" : row["client_name"]);
        item.set_client_sex(row["client_sex"].empty() ? 0 : atoi(row["client_sex"].c_str()));
        item.set_client_email(row["client_email"].empty() ? "" : row["client_email"]);
        item.set_client_qq(row["client_qq"].empty() ? "" : row["client_qq"]);
        item.set_client_msn(row["client_msn"].empty() ? "" : row["client_msn"]);
        item.set_client_fax(row["client_fax"].empty() ? "" : row["client_fax"]);
        item.set_client_type_id(row["client_type_id"].empty() ? 0 : atoi(row["client_type_id"].c_str()));
        item.set_fund_type_id(row["fund_type_id"].empty() ? 0 : atoi(row["fund_type_id"].c_str()));
        item.set_client_job_id(row["client_job_id"].empty() ? 0 : atoi(row["client_job_id"].c_str()));
        item.set_client_operate_id(row["client_operate_id"].empty() ? 0 : atoi(row["client_operate_id"].c_str()));
        item.set_province(row["province"].empty() ? "" : row["province"]);
        item.set_city(row["city"].empty() ? "" : row["city"]);
        item.set_client_from_id(row["client_from_id"].empty() ? 0 : atoi(row["client_from_id"].c_str()));
        item.set_address(row["address"].empty() ? "" : row["address"]);
        item.set_lock_employee_id(row["lock_employee_id"].empty() ? 0 : _atoi64(row["lock_employee_id"].c_str()));
        item.set_lock_employee_name(row["lock_employee_name"].empty() ? "" : row["lock_employee_name"]);
        item.set_remark(row["remark"].empty() ? "" : row["remark"]);
        item.set_client_tag(row["client_tag"].empty() ? "" : row["client_tag"]);
        item.set_user_type(row["user_type"].empty() ? 0 : atoi(row["user_type"].c_str()));
        item.set_registe_date(row["registe_date"].empty() ? "" : row["registe_date"]);
        item.set_last_record(row["last_record"].empty() ? "" : row["last_record"]);
        item.set_last_traced_date(row["last_traced_date"].empty() ? "" : row["last_traced_date"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

// 客户附加电话
DB_RESULT mysql_oa_table::request_client_phone_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_CLIENT_PHONE item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_client_phone(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_client_id(row["client_id"].empty() ? "" : row["client_id"]);
        item.set_client_phone(row["client_phone"].empty() ? "" : row["client_phone"]);
        item.set_is_mobile(row["is_mobile"].empty() ? 0 : atoi(row["is_mobile"].c_str()));
        item.set_is_main(row["is_main"].empty() ? 0 : atoi(row["is_main"].c_str()));
        item.set_create_id(row["create_id"].empty() ? 0 : atoi(row["create_id"].c_str()));
        item.set_create_name(row["create_name"].empty() ? "" : row["create_name"]);
        item.set_create_date(row["create_date"].empty() ? "" : row["create_date"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

// 客户附加信息
DB_RESULT mysql_oa_table::request_client_info_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_CLIENT_INFO item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_client_info(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_client_id(row["client_id"].empty() ? "" : row["client_id"]);
        item.set_info_type_id(row["info_type_id"].empty() ? 0 : atoi(row["info_type_id"].c_str()));
        item.set_info_type_name(row["client_info_type_nameid"].empty() ? "" : row["info_type_name"]);
        item.set_info(row["info"].empty() ? "" : row["info"]);
        item.set_create_id(row["create_id"].empty() ? 0 : _atoi64(row["create_id"].c_str()));
        item.set_create_name(row["create_name"].empty() ? "" : row["create_name"]);
        item.set_create_date(row["create_date"].empty() ? "" : row["create_date"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_client_favorite_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_CLIENT_FAVORITE_VIEWS item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_client_favorite_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_favorite_id(row["client_id"].empty() ? 0 : _atoi64(row["client_id"].c_str()));
        item.set_client_id(row["info_type_id"].empty() ? "" : row["info_type_id"]);
        item.set_favorite_type_id(row["favorite_type_id"].empty() ? 0 : atoi(row["favorite_type_id"].c_str()));
        item.set_client_name(row["client_name"].empty() ? "" : row["client_name"]);
        item.set_remark(row["remark"].empty() ? "" : row["remark"]);
        item.set_create_date(row["create_date"].empty() ? "" : row["create_date"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_client_record_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_CLIENT_RECORD_VIEWS item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_client_record_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_record_id(row["record_id"].empty() ? 0 : _atoi64(row["record_id"].c_str()));
        item.set_record_type_id(row["record_type_id"].empty() ? 0 : atoi(row["record_type_id"].c_str()));
        item.set_business_id(row["business_id"].empty() ? 0 : atoi(row["business_id"].c_str()));

        item.set_record_type_big_id(row["record_type_big_id"].empty() ? 0 : atoi(row["record_type_big_id"].c_str()));
        item.set_record_type_small_id(row["record_type_small_id"].empty() ? 0 : atoi(row["record_type_small_id"].c_str()));
        item.set_record_color_id(row["record_color_id"].empty() ? 0 : atoi(row["record_color_id"].c_str()));
        item.set_record_info(row["record_info"].empty() ? "" : row["record_info"]);
        item.set_create_id(row["create_id"].empty() ? 0 : atoi(row["create_id"].c_str()));

        item.set_create_name(row["create_name"].empty() ? "" : row["create_name"]);
        item.set_client_type_id(row["client_type_id"].empty() ? 0 : atoi(row["client_type_id"].c_str()));
        item.set_create_date(row["create_date"].empty() ? "" : row["create_date"]);
        item.set_record_open_date(row["record_open_date"].empty() ? "" : row["record_open_date"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_client_order_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_CLIENT_ORDER item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    records.set_record_type(proto.record_type());
    records.set_record_count(0);
    do 
    {
        if (RECORD_CLIENT_ORDER_COUNT != proto.record_type() &&
            RECORD_CLIENT_ORDER_VIEW_COUNT != proto.record_type())
        {
            break;
        }
        if (false == proto.has_retention() ||
            true == proto.retention().empty())
        {
            break;
        }
        INIT_MYSQL_OA_CONNECTION();
        query << proto.retention();
        mysqlpp::UseQueryResult use_res = query.use();
        if (false == use_res)
        {
            LLOG(ERR, "request_client_order_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
            break;
        }
        while (row = use_res.fetch_row())
        {
            std::string strcount = row[0].c_str();
            records.set_record_count(_atoi64(strcount.c_str()));
            break;
        }
    } while (0);

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_client_order_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        
        item.set_order_type_id(row["order_type_id"].empty() ? 0 : atoi(row["order_type_id"].c_str()));
        item.set_product_big_id(row["product_big_id"].empty() ? 0 : atoi(row["product_big_id"].c_str()));
        item.set_order_person_name(row["order_person_name"].empty() ? "" : row["order_person_name"]);
        item.set_product_small_id(row["product_small_id"].empty() ? 0 : atoi(row["product_small_id"].c_str()));
        item.set_order_status_id(row["order_status_id"].empty() ? 0 : atoi(row["order_status_id"].c_str()));
        item.set_order_amount(row["order_amount"].empty() ? 0.0 : atof(row["order_amount"].c_str()));
        item.set_order_person(row["order_person"].empty() ? "" : row["order_person"]);
        item.set_order_create_time(row["order_create_time"].empty() ? "" : row["order_create_time"]);
        item.set_order_active_time(row["order_active_time"].empty() ? "" : row["order_active_time"]);
        item.set_order_end_time(row["order_end_time"].empty() ? "" : row["order_end_time"]);


        item.set_order_id(row["order_id"].empty() ? 0 : _atoi64(row["order_id"].c_str()));
        item.set_client_id(row["client_id"].empty() ? "" : row["client_id"]);
        item.set_order_type_name(row["order_type_name"].empty() ? "" : row["order_type_name"]);
        item.set_order_type_id(row["order_type_id"].empty() ? 0 : atoi(row["order_type_id"].c_str()));
        item.set_product_big_id(row["product_big_id"].empty() ? 0 : atoi(row["product_big_id"].c_str()));
        item.set_product_class_big(row["product_class_big"].empty() ? "" : row["product_class_big"]);
        item.set_product_class_small(row["product_class_small"].empty() ? "" : row["product_class_small"]);
        item.set_order_person_name(row["order_person_name"].empty() ? "" : row["order_person_name"]);
        item.set_product_small_id(row["product_small_id"].empty() ? 0 : atoi(row["product_small_id"].c_str()));
        item.set_order_status_id(row["order_status_id"].empty() ? 0 : atoi(row["order_status_id"].c_str()));
        item.set_order_amount(row["order_amount"].empty() ? 0.0 : atof(row["order_amount"].c_str()));
        item.set_order_charge(row["order_charge"].empty() ? 0.0 : atof(row["order_charge"].c_str()));
        item.set_order_status_name(row["order_status_name"].empty() ? "" : row["order_status_name"]);
        item.set_order_person(row["order_person"].empty() ? "" : row["order_person"]);
        item.set_order_create_time(row["order_create_time"].empty() ? "" : row["order_create_time"]);
        item.set_order_active_time(row["order_active_time"].empty() ? "" : row["order_active_time"]);
        item.set_order_end_time(row["order_end_time"].empty() ? "" : row["order_end_time"]);
        item.set_order_remark(row["order_remark"].empty() ? "" : row["order_remark"]);
        item.set_order_open(row["order_open"].empty() ? 0 : atoi(row["rder_open"].c_str()));
        item.set_order_open_name(row["order_open_name"].empty() ? "" : row["order_open_name"]);
        item.set_order_open_date(row["order_open_date"].empty() ? "" : row["order_open_date"]);
        item.set_order_color_id(row["order_color_id"].empty() ? 0 : atoi(row["order_color_id"].c_str()));
        
        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_text_type_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_TEXT_TYPE item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_text_type_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_type_key(row["type_key"].empty() ? 0 : atoi(row["type_key"].c_str()));
        item.set_type_id(row["type_id"].empty() ? 0 : atoi(row["type_id"].c_str()));
        item.set_type_name(row["type_name"].empty() ? "" : row["type_name"]);
        item.set_item_id(row["item_id"].empty() ? 0 : atoi(row["item_id"].c_str()));
        item.set_item_text(row["item_text"].empty() ? "" : row["item_text"]);
        item.set_item_prev_id(row["item_prev_id"].empty() ? 0 : atoi(row["item_prev_id"].c_str()));

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_employee_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_EMPLOYEE item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_text_type_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_employee_id(row["employee_id"].empty() ? 0 : _atoi64(row["employee_id"].c_str()));
        item.set_employee_name(row["employee_name"].empty() ? "" : row["employee_name"]);
        item.set_employee_password(row["employee_password"].empty() ? "" : row["employee_password"]);
        item.set_employee_sex(row["employee_sex"].empty() ? 0 : atoi(row["employee_sex"].c_str()));
        item.set_employee_birthday(row["employee_birthday"].empty() ? "" : row["employee_birthday"]);
        item.set_employee_id_card(row["employee_id_card"].empty() ? "" : row["employee_id_card"]);
        item.set_employee_mobile(row["employee_mobile"].empty() ? "" : row["employee_mobile"]);
        item.set_employee_phone(row["employee_phone"].empty() ? "" : row["employee_phone"]);
        item.set_employee_email(row["employee_email"].empty() ? "" : row["employee_email"]);
        item.set_employee_degree_id(row["employee_degree_id"].empty() ? 0 : atoi(row["employee_degree_id"].c_str()));
        item.set_employee_qq(row["employee_qq"].empty() ? "" : row["employee_qq"]);
        item.set_employee_qq_name(row["employee_qq_name"].empty() ? "" : row["employee_qq_name"]);
        item.set_employee_addr(row["employee_addr"].empty() ? "" : row["employee_addr"]);
        item.set_agency_id(row["agency_id"].empty() ? 0 : atoi(row["agency_id"].c_str()));
        item.set_post_id(row["post_id"].empty() ? 0 : atoi(row["post_id"].c_str()));
        item.set_demo_account(row["demo_account"].empty() ? "" : row["demo_account"]);
        item.set_phone_num(row["phone_num"].empty() ? "" : row["phone_num"]);
        item.set_last_login_ip(row["last_login_ip"].empty() ? "" : row["last_login_ip"]);
        item.set_last_login_date(row["last_login_date"].empty() ? "" : row["last_login_date"]);
        item.set_is_delete(row["is_delete"].empty() ? 0 : atoi(row["is_delete"].c_str()));
        item.set_skill_id(row["skill_id"].empty() ? 0 : atoi(row["skill_id"].c_str()));
        item.set_remark(row["remark"].empty() ? "" : row["remark"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_server_type_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_SERVER_TYPE item;
    SQL_RECORDS records;
    mysqlpp::Row row;
    records.set_record_type(proto.record_type());
    records.set_record_count(0);
    records.set_record_begin(0);
    records.set_record_end(0);

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_text_type_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_server_type_key(row["server_type_key"].empty() ? 0 : atoi(row["server_type_key"].c_str()));
        item.set_server_type(row["server_type"].empty() ? "" : row["server_type"]);
        item.set_server_type_price(row["server_type_price"].empty() ? 0.0 : atof(row["server_type_price"].c_str()));
        item.set_is_delete(row["is_delete"].empty() ? 0 : atoi(row["is_delete"].c_str()));
        item.set_remark(row["remark"].empty() ? "" : row["remark"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_sms_template_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_SMS_TEMPLATE item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_text_type_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_id(row["id"].empty() ? 0 : _atoi64(row["id"].c_str()));
        item.set_template_class_id(row["template_class_id"].empty() ? "" : row["template_class_id"]);
        item.set_template_class_name(row["template_class_name"].empty() ? "" : row["template_class_name"]);
        item.set_template_title_name(row["template_title_name"].empty() ? "" : row["template_title_name"]);
        item.set_create_id(row["create_id"].empty() ? "" : row["create_id"]);
        item.set_create_date(row["create_date"].empty() ? "" : row["create_date"]);
        item.set_message(row["message"].empty() ? "" : row["message"]);
        item.set_is_pass(row["is_pass"].empty() ? 0 : atoi(row["is_pass"].c_str()));
        item.set_pass_oprater_id(row["pass_oprater_id"].empty() ? "" : row["pass_oprater_id"]);
        item.set_pass_date(row["pass_date"].empty() ? "" : row["pass_date"]);
        item.set_create_name(row["create_name"].empty() ? "" : row["create_name"]);
        item.set_pass_oprater_name(row["pass_oprater_name"].empty() ? "" : row["pass_oprater_name"]);
        item.set_remark(row["remark"].empty() ? "" : row["remark"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_sms_order_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    
    SQL_RECORDS records;
    mysqlpp::Row row;
    records.set_record_type(proto.record_type());
    records.set_record_count(0);
    do 
    {
        if (RECORD_SMS_ORDER_COUNT != proto.record_type())
        {
            break;
        }
        if (false == proto.has_retention() ||
            true == proto.retention().empty())
        {
            break;
        }
        INIT_MYSQL_OA_CONNECTION();
        query << proto.retention();
        mysqlpp::UseQueryResult use_res = query.use();
        if (false == use_res)
        {
            LLOG(ERR, "request_sms_order_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
            break;
        }
        while (row = use_res.fetch_row())
        {
            std::string strcount = row[0].c_str();
            records.set_record_count(_atoi64(strcount.c_str()));
            break;
        }
    } while (0);

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_text_type_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        OA_SMS_ORDER item;
        item.set_sms_id(row["sms_id"].empty() ? 0 : _atoi64(row["sms_id"].c_str()));
        item.set_operator_id(row["operator_id"].empty() ? "" : row["operator_id"]);
        item.set_send_mobile(row["send_mobile"].empty() ? "" : row["send_mobile"]);
        item.set_send_time(row["send_time"].empty() ? "" : row["send_time"]);
        item.set_message(row["message"].empty() ? "" : row["message"]);
        item.set_operator_name(row["operator_name"].empty() ? "" : row["operator_name"]);
        item.set_client_id(row["client_id"].empty() ? "" : row["client_id"]);
        item.set_client_name(row["client_name"].empty() ? "" : row["client_name"]);
        item.set_send_succeed(row["send_succeed"].empty() ? 0 : atoi(row["send_succeed"].c_str()));

        records.add_records(item.SerializeAsString());
    }

    records.set_record_type(proto.record_type());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());
    if (RECORD_SMS_ORDER_COUNT != proto.record_type())
    {
        records.set_record_count(records.records_size());
    }
    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::request_user_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result)
{
    if (true==proto.sql().empty())
    {
        proto_result.set_result_code(std::string("sql_empty"));
        return DR_FAIL;
    }
    OA_USER_VIEWS item;
    SQL_RECORDS records;
    mysqlpp::Row row;

    INIT_MYSQL_OA_CONNECTION();

    query << proto.sql();
    mysqlpp::UseQueryResult use_res = query.use();
    if (false == use_res)
    {
        proto_result.set_result_code(std::string(query.error()));
        LLOG(ERR, "request_text_type_select(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    while (row = use_res.fetch_row())
    {
        item.set_client_id(row["client_id"].empty() ? 0 : _atoi64(row["client_id"].c_str()));
        item.set_user_name(row["user_name"].empty() ? "" : row["user_name"]);
        item.set_login_id(row["login_id"].empty() ? "" : row["login_id"]);
        item.set_register_date(row["register_date"].empty() ? "" : row["register_date"]);
        item.set_user_type(row["user_type"].empty() ? 0 : atoi(row["user_type"].c_str()));
        item.set_server_type_id(row["server_type_id"].empty() ? 0 : atoi(row["server_type_id"].c_str()));
        item.set_server_end_date(row["server_end_date"].empty() ? "" : row["server_end_date"]);
        item.set_user_mobile(row["user_mobile"].empty() ? "" : row["user_mobile"]);
        item.set_province(row["province"].empty() ? "" : row["province"]);
        item.set_city(row["city"].empty() ? "" : row["city"]);
        item.set_lock_employee_id(row["lock_employee_id"].empty() ? 0 : _atoi64(row["lock_employee_id"].c_str()));
        item.set_lock_employee_name(row["lock_employee_name"].empty() ? "" : row["lock_employee_name"]);

        records.add_records(item.SerializeAsString());
        item.Clear();
    }

    records.set_record_type(proto.record_type());
    records.set_record_count(records.records_size());
    records.set_record_begin(proto.begin());
    records.set_record_end(proto.begin() + records.records_size());

    proto_result.set_result_data(records.SerializeAsString());

    return records.records_size() >= 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::oa_sms_order_insert(const SHORT_MESSAGE_SERVER& sms, const SMS_RECEIVER& receiver, uint64_t& smsid)
{
    INIT_MYSQL_OA_CONNECTION();

    query << "INSERT INTO jwkoa.oa_sms_order "
         << "(operator_id,send_mobile,send_time,message,operator_name,client_id,client_name,send_succeed) "
         << "VALUES(" << sms.operator_id() << ","
         << "'" << receiver.client_phone() << "',"
         << "'" << create_current_datetime() << "',"
         << "'" << sms.sms_msg() << "',"
         << "'" << sms.operator_name() << "',"
         << "'" << receiver.client_id() << "',"
         << "'" << receiver.client_name() << "',"
         << 0 << ");";

    mysqlpp::SimpleResult res = query.execute();
    if (false == res)
    {
        LLOG(ERR, "oa_sms_order_insert(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    smsid = res.insert_id();
    return DR_SUCCEED;
}

DB_RESULT mysql_oa_table::oa_sms_order_update(const uint64_t smsid, const int isuccess)
{
    INIT_MYSQL_OA_CONNECTION();

    query << "UPDATE oa_sms_order SET send_succeed=" << isuccess << " "
          << "WHERE sms_id=" << smsid << ";";

    mysqlpp::SimpleResult res = query.execute();
    if (false == res)
    {
        LLOG(ERR, "oa_sms_order_update(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }
    return res.rows() > 0 ? DR_SUCCEED : DR_EMPTY;
}

DB_RESULT mysql_oa_table::get_oa_client_max_client_id(std::string& clientid)
{
    INIT_MYSQL_OA_CONNECTION();

    query << "SELECT MAX(client_id) FROM oa_client;";

    mysqlpp::UseQueryResult res = query.use();
    if (false == res)
    {
        LLOG(ERR, "get_oa_client_last_client_id(db err_nun:%d err_code:%s)", query.errnum(), query.error());
        return DR_FAIL;
    }

    mysqlpp::Row row;
    while (row = res.fetch_row())
    {
        clientid = row[0];
        break;
    }

    return false==clientid.empty() ? DR_SUCCEED : DR_EMPTY;
}