#include <stdio.h>
#include "sai.h"
#include "sailag.h"

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

int main()
{
    sai_status_t              status;
    sai_switch_api_t          *switch_api;
    sai_lag_api_t             *lag_api_instance;
    sai_object_id_t           vr_oid;
    sai_attribute_t           attrs[2];
    sai_switch_notification_t notifications;
    sai_object_id_t           port_list[64];

    // Init LAG API
    status = sai_api_initialize(0, &test_services);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to initialize SAI, status=%d\n", status);
        return 1;
    }
    status = sai_api_query(SAI_API_LAG, (void**)&lag_api_instance);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to query LAG API, status=%d\n", status);
        return 1;
    }

    // Create LAG1 with members
    sai_object_id_t lag_oid1;
    status = lag_api_instance->create_lag(&lag_oid1, 0, NULL);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG, status=%d\n", status);
        return 1;
    }

    sai_object_id_t lag_mem_id1;
    sai_object_id_t lag_mem_id2;
    status = lag_api_instance->create_lag_member(&lag_mem_id1, lag_oid1>>32, 0);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
    }
    status = lag_api_instance->create_lag_member(&lag_mem_id2, lag_oid1>>32, 0);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
    }

    // Create LAG2 with members
    sai_object_id_t lag_oid2;
    status = lag_api_instance->create_lag(&lag_oid2, 0, NULL);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG, status=%d\n", status);
        return 1;
    }

    sai_object_id_t lag_mem_id3;
    sai_object_id_t lag_mem_id4;
    status = lag_api_instance->create_lag_member(&lag_mem_id3, lag_oid2>>32, 0);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
    }
    status = lag_api_instance->create_lag_member(&lag_mem_id4, lag_oid2>>32, 0);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
    }

    // Remove LAG instances
    lag_api_instance->remove_lag_member(lag_mem_id2);
    lag_api_instance->remove_lag_member(lag_mem_id3);
    lag_api_instance->remove_lag_member(lag_mem_id1);
    lag_api_instance->remove_lag_member(lag_mem_id4);

    lag_api_instance->remove_lag(lag_oid2);
    lag_api_instance->remove_lag(lag_oid1);

    status = sai_api_uninitialize();

    return 0;
}