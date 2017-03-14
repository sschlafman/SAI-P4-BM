#ifndef SWITCH_META_DATA_H
#define SWITCH_META_DATA_H

#include  <sai.h>
#include <list>
#include <iostream>
#include <vector>
#include <map>
#include <standard_types.h>
using namespace bm_runtime::standard;

class sai_id_map_t { // object pointer and it's id
  protected:
    std::map<sai_object_id_t,void*> id_map;
    std::vector<sai_object_id_t> unused_id;
  public:
    sai_id_map_t(){
      //printf("sai_id_map_constructor \n",id_map.size(),unused_id.size());
      // init
      unused_id.clear();
      id_map.clear();
    }

    ~sai_id_map_t(){
      printf("sai_id_map_destructor, id_map addr: %d \n",&id_map);
    }

    void free_id(sai_object_id_t sai_object_id){
      printf("freeing object with sai_id %d\n",sai_object_id);
      delete id_map[sai_object_id];
      id_map.erase(sai_object_id);
      unused_id.push_back(sai_object_id);
    }

    sai_object_id_t get_new_id(void* obj_ptr){//pointer to object
      //printf("get_new_id\n");
      sai_object_id_t id;
      if(!unused_id.empty()){
        id = unused_id.back();
        unused_id.pop_back();
      }
      else { 
        id = id_map.size(); }
      //printf("id_map.size = %d \n", id_map.size());
      id_map[id]=obj_ptr;
      //printf("after insertion : id_map.size = %d \n", id_map.size());
      return id;
    }

    void *get_object(sai_object_id_t sai_object_id) {
      return id_map[sai_object_id];
    }
};


class Sai_obj {
  public:
    sai_object_id_t sai_object_id; // TODO maybe use the map and don't save here
    Sai_obj(sai_id_map_t* sai_id_map_ptr){
      sai_object_id = sai_id_map_ptr->get_new_id(this); // sai_id_map. set map to true.
    }
    ~Sai_obj(){
      //free_id(sai_object_id); TODO: fix this
    }
  
};


class Port_obj : public Sai_obj{
  public:
    uint32_t hw_port;
    uint32_t l2_if;
    uint32_t pvid;
    uint32_t bind_mode;
    uint32_t mtu;
    uint32_t drop_tagged;
    uint32_t drop_untagged;
    bool is_default;
    bool is_lag;
    BmEntryHandle handle_lag_if;
    BmEntryHandle handle_port_cfg;
    BmEntryHandle handle_ingress_lag;
    BmEntryHandle handle_egress_lag;
    Port_obj(sai_id_map_t* sai_id_map_ptr): Sai_obj(sai_id_map_ptr) {
      //printf("create port object\n");
      this->mtu=1512;
      this->drop_tagged=0;
      this->drop_untagged=0;
      this->hw_port=0;
      this->l2_if=0;
      this->pvid=1;
      this->bind_mode=SAI_PORT_BIND_MODE_PORT;
      this->is_default=true;
      this->is_lag=false;
      this->handle_ingress_lag = 999;
      this->handle_egress_lag = 999;
      this->handle_port_cfg = 999;
      this->handle_lag_if = 999;
    }   
};

class BridgePort_obj : public Sai_obj {
public:
  uint32_t port_id;
  uint32_t vlan_id;
  uint32_t bridge_port;
  sai_bridge_port_type_t bridge_port_type;
  sai_object_id_t bridge_id;
  BmEntryHandle handle_id_1d;
  BmEntryHandle handle_id_1q;
  BmEntryHandle handle_egress_set_vlan;
  BmEntryHandle handle_egress_br_port_to_if;
  BmEntryHandle handle_subport_ingress_interface_type;
  BmEntryHandle handle_port_ingress_interface_type;
  //BmEntryHandle handle_cfg; // TODO
  BridgePort_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->port_id=0;
    this->vlan_id=1;
    this->bridge_port=0;
    this->bridge_id=0;
    this->bridge_port_type=SAI_BRIDGE_PORT_TYPE_PORT;
    // TODO 999 is inavlid. consider other notation
    this->handle_id_1d =999;
    this->handle_id_1q =999;
    this->handle_egress_set_vlan =999;
    this->handle_egress_br_port_to_if =999;
    this->handle_subport_ingress_interface_type =999;
    this->handle_port_ingress_interface_type =999;

  }
};

class Bridge_obj : public Sai_obj {
public:
  sai_bridge_type_t bridge_type;
  std::vector<sai_object_id_t> bridge_port_list;
  uint32_t bridge_id;
  Bridge_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
    this->bridge_type=SAI_BRIDGE_TYPE_1Q;
    this->bridge_port_list.clear();
    this->bridge_id=1;
  }
};

class Vlan_obj : public Sai_obj {
  public:
    uint16_t vid;
    std::vector<sai_object_id_t> vlan_members;
    Vlan_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr) {
      this->vlan_members.clear();
      this->vid = 0;
    }
};

class Vlan_member_obj : public Sai_obj {
  public:
    sai_object_id_t bridge_port_id;
    sai_object_id_t vlan_oid;
    uint32_t tagging_mode;
    uint16_t vid;
    BmEntryHandle handle_egress_vlan_tag;
    BmEntryHandle handle_egress_vlan_filtering;
    BmEntryHandle handle_ingress_vlan_filtering;
    Vlan_member_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr){
      this->vid = 999;
      this->vlan_oid = 999;// TODO needed? consider remove.
      this->tagging_mode = SAI_VLAN_TAGGING_MODE_UNTAGGED;
      this->bridge_port_id=999;
    }
};

class Lag_obj : public Sai_obj {
public:
  uint32_t l2_if;
  std::vector<sai_object_id_t> lag_members;
  BmEntryHandle handle_lag_hash;
  BmEntryHandle handle_port_configurations;
  Port_obj* port_obj;
  Lag_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr){
    this->lag_members.clear();
    this->l2_if=0;
    this->port_obj=NULL;
    this->handle_lag_hash=999;
    this->handle_port_configurations=999;
  }
};

class Lag_member_obj : public Sai_obj{
  public:
    sai_object_id_t port_id;
    sai_object_id_t lag_id;
    uint32_t hw_port;
    Lag_member_obj(sai_id_map_t* sai_id_map_ptr) : Sai_obj(sai_id_map_ptr){
      this->port_id=0;
      this->lag_id=0;
      this->hw_port=0;
  }
};

typedef std::map<sai_object_id_t, BridgePort_obj*>  bridge_port_id_map_t;
typedef std::map<sai_object_id_t, Port_obj*>        port_id_map_t;
typedef std::map<sai_object_id_t, Bridge_obj*>      bridge_id_map_t;
typedef std::map<sai_object_id_t, Vlan_obj*>        vlan_id_map_t;
typedef std::map<sai_object_id_t, Vlan_member_obj*> vlan_member_id_map_t;
typedef std::map<sai_object_id_t, Lag_obj*>         lag_id_map_t;
typedef std::map<sai_object_id_t, uint32_t>         l2_if_map_t;
typedef std::map<sai_object_id_t, Lag_member_obj*>  lag_member_id_map_t;
class Switch_metadata { // TODO:  add default.. // this object_id is the switch_id
public:
  sai_u32_list_t        hw_port_list;
  port_id_map_t         ports;
  bridge_port_id_map_t  bridge_ports;
  bridge_id_map_t       bridges;
  vlan_id_map_t         vlans;
  vlan_member_id_map_t  vlan_members;
  lag_id_map_t          lags;
  lag_member_id_map_t   lag_members;
  sai_object_id_t       default_bridge_id;
  l2_if_map_t           l2_ifs; // TODO consider list
  Switch_metadata(){
    ports.clear();
    bridge_ports.clear();
    bridges.clear();
    vlans.clear();
    vlan_members.clear();
    lags.clear();
  }

  uint32_t GetNewBridgePort() {
    std::vector<uint32_t> bridge_port_nums;
    for (bridge_port_id_map_t::iterator it=bridge_ports.begin(); it!=bridge_ports.end(); ++it) {
      bridge_port_nums.push_back(it->second->bridge_port);
    }
    for (int i=0; i<bridge_port_nums.size(); ++i) {
      if (std::find(bridge_port_nums.begin(), bridge_port_nums.end(), i) == bridge_port_nums.end()) {
        return i;
      }
    }
    return bridge_port_nums.size();
  }

  uint32_t GetNewBridgeID() {
    std::vector<uint32_t> bridge_ids;
    for (bridge_id_map_t::iterator it=bridges.begin(); it!=bridges.end(); ++it) {
      bridge_ids.push_back(it->second->bridge_id);
    }
    for (int i=0; i<bridge_ids.size(); ++i) {
      if (std::find(bridge_ids.begin(), bridge_ids.end(), i) == bridge_ids.end()) {
        return i;
      }
    }
    return bridge_ids.size();
  }
  uint32_t GetNewL2IF() {
    std::vector<uint32_t> l2_ifs_nums;
    for (l2_if_map_t::iterator it=l2_ifs.begin(); it!=l2_ifs.end(); ++it) {
      l2_ifs_nums.push_back(it->second);
    }
    for (int i=0; i<l2_ifs_nums.size(); ++i) {
      if (std::find(l2_ifs_nums.begin(), l2_ifs_nums.end(), i) == l2_ifs_nums.end()) {
        return i;
      }
    }
    return l2_ifs_nums.size();
  }
};

#endif

