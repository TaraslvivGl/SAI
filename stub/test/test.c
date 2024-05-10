#include <stdio.h>
#include "sai.h"

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
    sai_switch_api_t         *switch_api;
    sai_object_id_t           vr_oid;
    sai_attribute_t           attrs[2];
    sai_switch_notification_t notifications;
    sai_object_id_t           port_list[64];

    status = sai_api_initialize(0, &test_services);
    status = sai_api_query(SAI_API_SWITCH, (void**)&switch_api);
    status = switch_api->initialize_switch(0, "HW_ID", 0, &notifications);
    attrs[0].id = SAI_SWITCH_ATTR_PORT_LIST;
    attrs[0].value.objlist.count = 64;
    attrs[0].value.objlist.list = port_list;
    status = switch_api->get_switch_attribute(1, attrs);
    for (int32_t ii = 0; ii < attrs[0].value.objlist.count; ii++) {
        printf("Port #%d OID: 0x%lX\n", ii, attrs[0].value.objlist.list[ii]);
    }
    switch_api->shutdown_switch(0);
    status = sai_api_uninitialize();

    return 0;
}