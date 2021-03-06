#include "../inc/sai_adapter.h"

sai_status_t sai_adapter::create_switch(sai_object_id_t *switch_id,
                                        uint32_t attr_count,
                                        const sai_attribute_t *attr_list) {
  (*logger)->info("create switch");
  if (switch_list_ptr->size() != 0) {
    (*logger)->debug(
        "currently one switch is supportred, returning operating switch_id: {}",
        (*switch_list_ptr)[0]);
    return (*switch_list_ptr)[0];
  } else {
    BmAddEntryOptions options;
    BmMatchParams match_params;
    BmMtEntry entry;
    BmActionData action_data;

    // Create Default 1Q Bridge, Vlan and switch_obj (not sure if switch is needed).
    Bridge_obj *bridge = new Bridge_obj(sai_id_map_ptr);
    (*logger)->info("Default 1Q bridge. sai_object_id {} bridge_id {}",
                    bridge->sai_object_id, bridge->bridge_id);
    Sai_obj *switch_obj = new Sai_obj(sai_id_map_ptr);
    switch_metadata_ptr->bridges[bridge->sai_object_id] = bridge;
    switch_metadata_ptr->default_bridge_id = bridge->sai_object_id;

    Vlan_obj *vlan = new Vlan_obj(sai_id_map_ptr);
    switch_metadata_ptr->vlans[vlan->sai_object_id] = vlan;
    vlan->vid = 1;
    vlan->bridge_id = switch_metadata_ptr->GetNewBridgeID(vlan->vid);
    vlan->handle_mc_mgrp = 0;
    vlan->handle_mc_l1 = 0;
    match_params.push_back(parse_exact_match_param(vlan->bridge_id, 2));
    // bm_bridge_client_ptr->bm_mt_get_entry_from_key(entry, cxt_id, "table_broadcast", match_params, options);
    // vlan->handle_broadcast = entry.entry_handle;
    // bm_bridge_client_ptr->bm_mt_get_entry_from_key(entry, cxt_id, "table_flood", match_params, options);
    // vlan->handle_flood = entry.entry_handle;
    switch_metadata_ptr->default_vlan_oid = vlan->sai_object_id;

    for (int i = 0; i < switch_metadata_ptr->hw_port_list.count; i++) {
      int hw_port = switch_metadata_ptr->hw_port_list.list[i];

      // Create Default Ports (one for each hw_port)
      Port_obj *port = new Port_obj(sai_id_map_ptr);
      switch_metadata_ptr->ports[port->sai_object_id] = port;
      port->hw_port = hw_port;
      port->l2_if = hw_port;
      (*logger)->info("Default port_id {}. hw_port = {}", port->sai_object_id,
                      port->hw_port);

      // Create Default Bridge ports and connect to 1Q bridge
      BridgePort_obj *bridge_port = new BridgePort_obj(sai_id_map_ptr);
      switch_metadata_ptr->bridge_ports[bridge_port->sai_object_id] =
          bridge_port;
      bridge_port->port_id = port->sai_object_id;
      bridge_port->bridge_port = hw_port;
      bridge_port->bridge_id = bridge->sai_object_id;
      bridge->bridge_port_list.push_back(bridge_port->sai_object_id);
      (*logger)->info("Default bridge_port_id {}. bridge_port = {}",
                      bridge_port->sai_object_id, bridge_port->bridge_port);

      // add default bridge ports to default vlan
      Vlan_member_obj *vlan_member = new Vlan_member_obj(sai_id_map_ptr);
      switch_metadata_ptr->vlan_members[vlan_member->sai_object_id] = vlan_member;
      vlan_member->vlan_oid = switch_metadata_ptr->default_vlan_oid;
      vlan_member->vid = vlan->vid;
      vlan_member->bridge_port_id = bridge_port->sai_object_id;
      vlan_member->tagging_mode = SAI_VLAN_TAGGING_MODE_UNTAGGED;
      vlan->vlan_members.push_back(vlan_member->sai_object_id);

      // Store default table entries
      match_params.clear();
      match_params.push_back(parse_exact_match_param(port->l2_if, 1));
      bm_bridge_client_ptr->bm_mt_get_entry_from_key(
          entry, cxt_id, "table_port_ingress_interface_type", match_params,
          options);
      bridge_port->handle_port_ingress_interface_type = entry.entry_handle;
      match_params.clear();
      match_params.push_back(
          parse_exact_match_param(bridge_port->bridge_port, 1));
      bm_bridge_client_ptr->bm_mt_get_entry_from_key(
          entry, cxt_id, "table_egress_br_port_to_if", match_params, options);
      bridge_port->handle_egress_br_port_to_if = entry.entry_handle;

      match_params.clear();
      match_params.push_back(parse_exact_match_param(port->l2_if, 1));
      match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
      match_params.push_back(parse_valid_match_param(true));
      bm_bridge_client_ptr->bm_mt_get_entry_from_key(
          entry, cxt_id, "table_egress_vlan_tag", match_params, options);
      vlan_member->handle_egress_vlan_tag = entry.entry_handle;
      match_params.clear();
      match_params.push_back(parse_exact_match_param(bridge_port->bridge_port, 1));
      match_params.push_back(parse_exact_match_param(vlan_member->vid, 2));
      bm_bridge_client_ptr->bm_mt_get_entry_from_key(
          entry, cxt_id, "table_egress_vlan_filtering", match_params, options);
      vlan_member->handle_egress_vlan_filtering = entry.entry_handle;
      bm_bridge_client_ptr->bm_mt_get_entry_from_key(
          entry, cxt_id, "table_ingress_vlan_filtering", match_params, options);
      vlan_member->handle_ingress_vlan_filtering = entry.entry_handle;

    }

    // Create Bridge Router port and bridge_port
      Port_obj *port = new Port_obj(sai_id_map_ptr);
      switch_metadata_ptr->ports[port->sai_object_id] = port;
      port->hw_port = 9;
      port->l2_if = 9;
      (*logger)->info("Router port_id {}. hw_port = {}", port->sai_object_id,
                      port->hw_port);
      BridgePort_obj *bridge_port = new BridgePort_obj(sai_id_map_ptr);
      switch_metadata_ptr->bridge_ports[bridge_port->sai_object_id] =
          bridge_port;
      switch_metadata_ptr->router_bridge_port = bridge_port;
      bridge_port->bridge_port_type = SAI_BRIDGE_PORT_TYPE_1Q_ROUTER;
      bridge_port->port_id = port->sai_object_id;
      bridge_port->bridge_port = port->hw_port;
      bridge_port->bridge_id = bridge->sai_object_id;
      bridge->bridge_port_list.push_back(bridge_port->sai_object_id);
      (*logger)->info("Router bridge_port_id {}. bridge_port = {}",
                      bridge_port->sai_object_id, bridge_port->bridge_port);
      // Store default table entries
      // match_params.clear();
      // match_params.push_back(parse_exact_match_param(port->l2_if, 1));
      // bridge_port->handle_port_ingress_interface_type = bm_bridge_client_ptr->bm_mt_add_entr(
          // entry, cxt_id, "table_port_ingress_interface_type", match_params,
          // options);
      match_params.clear();
      match_params.push_back(parse_exact_match_param(bridge_port->bridge_port, 1));
      action_data.clear();
      action_data.push_back(parse_param(port->hw_port, 1));
      action_data.push_back(parse_param(2,1)); // 2 - out_if_type == ROUTER
      // table_add table_egress_br_port_to_if action_forward_set_outIfType 0 => 0 0

      bridge_port->handle_egress_br_port_to_if = bm_bridge_client_ptr->bm_mt_add_entry(cxt_id, "table_egress_br_port_to_if", match_params, "action_forward_set_outIfType", action_data, options);


    // Create MAC learn hostif_table_entry
    HostIF_Table_Entry_obj *hostif_table_entry =
        new HostIF_Table_Entry_obj(sai_id_map_ptr);
    switch_metadata_ptr
        ->hostif_table_entries[hostif_table_entry->sai_object_id] =
        hostif_table_entry;
    hostif_table_entry->entry_type = SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID;
    hostif_table_entry->trap_id = MAC_LEARN_TRAP_ID;
    add_hostif_trap_id_table_entry(MAC_LEARN_TRAP_ID, learn_mac);

    switch_list_ptr->push_back(switch_obj->sai_object_id);
    return switch_obj->sai_object_id;
  }
}

sai_status_t sai_adapter::get_switch_attribute(sai_object_id_t switch_id,
                                               sai_uint32_t attr_count,
                                               sai_attribute_t *attr_list) {
  (*logger)->info("get_switch_attribute");
  int i;
  int index = 0;
  for (i = 0; i < attr_count; i++) {
    switch ((attr_list + i)->id) {
    case SAI_SWITCH_ATTR_DEFAULT_1Q_BRIDGE_ID:
      (attr_list + i)->value.oid = switch_metadata_ptr->default_bridge_id;
      break;
    case SAI_SWITCH_ATTR_PORT_LIST:
      for (port_id_map_t::iterator it = switch_metadata_ptr->ports.begin();
           it != switch_metadata_ptr->ports.end(); ++it) {
        (attr_list + i)->value.objlist.list[index] = it->first;
        index += 1;
      }
      break;
    case SAI_SWITCH_ATTR_PORT_NUMBER:
      (attr_list + i)->value.u32 = switch_metadata_ptr->hw_port_list.count;
      break;
    case SAI_SWITCH_ATTR_DEFAULT_VLAN_ID:
      (attr_list + i)->value.oid = switch_metadata_ptr->default_vlan_oid;
      break;
    }
  }
  return SAI_STATUS_SUCCESS;
}

sai_status_t sai_adapter::set_switch_attribute(sai_object_id_t switch_id, const sai_attribute_t *attr) {
  (*logger)->info("set_switch_attribute");
  switch (attr->id) {
    case SAI_SWITCH_ATTR_SRC_MAC_ADDRESS:
      memcpy(switch_metadata_ptr->default_switch_mac, attr->value.mac, 6);
      (*logger)->info("default switch mac set to:");
      print_mac_to_log(switch_metadata_ptr->default_switch_mac, *logger);
      break;
  }
  return SAI_STATUS_SUCCESS;
}
