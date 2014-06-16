#ifndef __MYSQL_OA_TABLES_H__
#define __MYSQL_OA_TABLES_H__

#pragma once

#include "mysql++.h"
#include "mysql_type.h"

#include "OfficeSystem.pb.h"

struct user_verify_info 
{
    int32_t is_delete;
    int32_t skill_id;
    std::string employee_password;
    std::string employee_name;
    std::string agency_name;
    std::string post_name;
    std::string post_privileges;
    std::string demo_account;
};

class mysql_oa_table
{
public:
    mysql_oa_table(void);
    ~mysql_oa_table(void);

    // 执行sql查询语句
    DB_RESULT query_sql(const std::string& strsql, uint64_t& urows, std::string& errcode);

    // 获用户验证信息
    DB_RESULT get_user_verify_info(const std::string& userid, user_verify_info& info);

    // sql语句执行返回结果
    DB_RESULT request_insert_update_delete(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 职务列表
    DB_RESULT request_post_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 机构数据
    DB_RESULT request_agency_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 客户数据
    DB_RESULT request_client_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 客户附加电话
    DB_RESULT request_client_phone_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 客户附加信息
    DB_RESULT request_client_info_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 收藏夹
    DB_RESULT request_client_favorite_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 工单
    DB_RESULT request_client_record_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 客户订单
    DB_RESULT request_client_order_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 文本类型
    DB_RESULT request_text_type_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 员工
    DB_RESULT request_employee_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 服务类型列表
    DB_RESULT request_server_type_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 短信模板
    DB_RESULT request_sms_template_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 短信订单
    DB_RESULT request_sms_order_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // 短信订单
    DB_RESULT request_user_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);

    // 插入短信
    DB_RESULT oa_sms_order_insert(const SHORT_MESSAGE_SERVER& sms, const SMS_RECEIVER& receiver, uint64_t& smsid);
    // 列新短信发送状态
    DB_RESULT oa_sms_order_update(const uint64_t smsid, const int isuccess);

    // oa_client最大的客户ID以便从外网导入新记录
    DB_RESULT get_oa_client_max_client_id(std::string& clientid);
};

#endif  //__MYSQL_OA_TABLES_H__