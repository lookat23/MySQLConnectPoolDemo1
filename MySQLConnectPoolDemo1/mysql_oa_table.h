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

    // ִ��sql��ѯ���
    DB_RESULT query_sql(const std::string& strsql, uint64_t& urows, std::string& errcode);

    // ���û���֤��Ϣ
    DB_RESULT get_user_verify_info(const std::string& userid, user_verify_info& info);

    // sql���ִ�з��ؽ��
    DB_RESULT request_insert_update_delete(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // ְ���б�
    DB_RESULT request_post_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // ��������
    DB_RESULT request_agency_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // �ͻ�����
    DB_RESULT request_client_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // �ͻ����ӵ绰
    DB_RESULT request_client_phone_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // �ͻ�������Ϣ
    DB_RESULT request_client_info_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // �ղؼ�
    DB_RESULT request_client_favorite_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // ����
    DB_RESULT request_client_record_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // �ͻ�����
    DB_RESULT request_client_order_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // �ı�����
    DB_RESULT request_text_type_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // Ա��
    DB_RESULT request_employee_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // ���������б�
    DB_RESULT request_server_type_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // ����ģ��
    DB_RESULT request_sms_template_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // ���Ŷ���
    DB_RESULT request_sms_order_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);
    // ���Ŷ���
    DB_RESULT request_user_select(const PARAM_SQL& proto, REQUEST_DATA_RESULT& proto_result);

    // �������
    DB_RESULT oa_sms_order_insert(const SHORT_MESSAGE_SERVER& sms, const SMS_RECEIVER& receiver, uint64_t& smsid);
    // ���¶��ŷ���״̬
    DB_RESULT oa_sms_order_update(const uint64_t smsid, const int isuccess);

    // oa_client���Ŀͻ�ID�Ա�����������¼�¼
    DB_RESULT get_oa_client_max_client_id(std::string& clientid);
};

#endif  //__MYSQL_OA_TABLES_H__