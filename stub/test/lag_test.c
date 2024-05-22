#include <stdio.h>
#include <string.h>
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

#define PORT_NUMBER 32

// PORT test API
int ports[PORT_NUMBER];
void reset_ports()
{
    memset(ports, 0, sizeof(ports));
}

// -1 if there is no free ports
sai_status_t take_free_port(sai_object_id_t *port_oid)
{
    for(int i = 0; i < PORT_NUMBER; ++i)
    {
        if (ports[i] == 0)
        {
            if (stub_create_object(SAI_OBJECT_TYPE_PORT, i, port_oid) == SAI_STATUS_SUCCESS)
            {
                ports[i] = 1;
                return SAI_STATUS_SUCCESS;
            }
            printf("Failed to create PORT object\n");
            return SAI_STATUS_FAILURE;
        }
    }
    printf("No free ports\n");
    return SAI_STATUS_FAILURE;
}

void free_port(sai_object_id_t port_oid)
{
    uint32_t port_number;
    sai_status_t status = stub_object_to_type(port_oid, SAI_OBJECT_TYPE_PORT, &port_number);
    if (status == SAI_STATUS_SUCCESS)
    {
        printf("Free port #%d\n", port_number);
    }
    else
    {
        printf("Failed to port #%d\n", port_number);
    }
    
    if (port_number < PORT_NUMBER)
    {
        ports[port_number] = 0;
    }
    else
    {
        printf("Wrong port number %d\n", port_number);
    }
}
// PORT END

int main()
{
    sai_status_t              status;
    sai_switch_api_t          *switch_api;
    sai_lag_api_t             *lag_api_instance;
    sai_object_id_t           vr_oid;
    sai_attribute_t           attrs[2];
    sai_switch_notification_t notifications;
    sai_object_id_t           port_list[64];

    reset_ports();

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

    sai_object_id_t lag_mem_id1, lag_mem_id2;
    sai_object_id_t lag_port1,lag_port2;
    take_free_port(&lag_port1);
    take_free_port(&lag_port2);

    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag_oid1;
    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = lag_port1;

    status = lag_api_instance->create_lag_member(&lag_mem_id1, 2, attrs);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
    }

    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag_oid1;
    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = lag_port2;

    status = lag_api_instance->create_lag_member(&lag_mem_id2, 2, attrs);
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

    sai_object_id_t lag_mem_id3, lag_mem_id4;
    sai_object_id_t lag_port3,lag_port4;
    take_free_port(&lag_port3);
    take_free_port(&lag_port4);

    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag_oid2;
    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = lag_port3;

    status = lag_api_instance->create_lag_member(&lag_mem_id3, 2, attrs);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
    }

    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag_oid2;
    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = lag_port4;

    status = lag_api_instance->create_lag_member(&lag_mem_id4, 2, attrs);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
    }

    printf("---- LAG attributes--------\n");  
    const uint32_t port_number = 16;
    sai_object_id_t port_objects[port_number];
    memset(port_objects, 0, sizeof(port_objects));
    sai_attribute_t port_attrs;
    port_attrs.id = SAI_LAG_ATTR_PORT_LIST;
    port_attrs.value.objlist.list = port_objects;
    status = lag_api_instance->get_lag_attribute(lag_oid2, 1, &port_attrs);

    if (status == SAI_STATUS_SUCCESS)
    {
        printf("LAG 0x%lX includes:\n", lag_oid2);
        for (int i = 0; i < port_attrs.value.objlist.count; ++i)
        {
            printf("Port#%d: 0x%lX\n", i, port_attrs.value.objlist.list[i]);
        }
    }
    else
    {
        printf("Failed to get port list %d\n", status);
    }

    printf("---- LAG member attributes--------\n"); 
    sai_attribute_t lag_mem_attrs;
    lag_mem_attrs.id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    status = lag_api_instance->get_lag_member_attribute(lag_mem_id2, 1, &lag_mem_attrs);
    if (status == SAI_STATUS_SUCCESS)
    {
        printf("LAG MEMBER 0x%lX has LAG 0x%lX\n", lag_mem_id2, lag_mem_attrs.value.oid);
    }
    else
    {
        printf("Failed to get lag memeber attr %d\n", status);
    }

    // Try to remove when lag members are still exists
    lag_api_instance->remove_lag(lag_oid1);
    
    // Remove LAG instances
    lag_api_instance->remove_lag_member(lag_mem_id2);
    lag_api_instance->remove_lag_member(lag_mem_id1);
    
    lag_api_instance->remove_lag_member(lag_mem_id3);
    lag_api_instance->remove_lag_member(lag_mem_id4);

    lag_api_instance->remove_lag(lag_oid2);
    lag_api_instance->remove_lag(lag_oid1);

    status = sai_api_uninitialize();

    return 0;
}