
using namespace eosio;

/**
 * @brief      Модуль IPFS
 * Позволяет хранить ключи доступа к интерпланетарной файловой системе, 
 * создавать публичные пути орбитальной базы данных, 
 * а так же покупать и продавать зашифрованные данные. 
 */
struct ipfs {


  void setstorage_action(const setstorage &op){
    require_auth(op.username);

    storage_index storages(_self, op.username.value);
    auto storage = storages.find(op.id);
    
    if (storage == storages.end()){
      
      storages.emplace(op.username, [&](auto &s){
        s.id = op.id;
        s.address = op.address;
        s.name = op.name;
      });

    } else {
      storages.modify(storage, op.username, [&](auto &s){
        s.address = op.address;
        s.name = op.name;
      });

    };
  }

  void removeroute_action(const removeroute &op){
    require_auth(op.username);
    storage_index storages(_self, op.username.value);
    auto storage = storages.find(op.id);
    storages.erase(storage);
  }

  void setipfskey_action(const setipfskey &op){
    require_auth(op.username);

    ipfskeys_index ipfskeys(_self, op.username.value);
    
    ipfskeys.emplace(op.username, [&](auto &i){
      i.id = ipfskeys.available_primary_key();
      i.pkey = op.pkey;
      i.meta = op.meta;
    });
  }



  void selldata_action(const selldata &op){
    require_auth(op.username);

    orbdata_index orbdts(_self, op.username.value);
    auto data = orbdts.find(op.id);
    check(data == orbdts.end(), "Cannot use same ID");
    auto failure_if_root_not_exist = eosio::token(op.root_token_contract).get_supply(eosio::symbol_code(op.amount.symbol).name()).amount;

    orbdts.emplace(op.username, [&](auto &ds){
      ds.id = op.id;
      ds.data = op.data;
      ds.root_token_contract = op.root_token_contract;
      ds.amount = op.amount;
    });
  }

  void buydata_action(eosio::name owner, uint64_t data_id, eosio::name buyer, eosio::asset quantity, eosio::name code){
    
    orbdata_index orbdts(_self, owner.value);
    auto data = orbdts.find(data_id);
    check(data != orbdts.end(), "Data ID is not found. Rejected.");

    check(data -> root_token_contract == code, "Wrong token contract for this data unit. Rejected");
    check(data -> amount == quantity, "Recieved amount is not equal with data cost. Rejected");

    dataorders_index oorders(_self, owner.value);

    //TODO check for order exist by data_id and buyer;


    auto buyers_with_data_id = oorders.template get_index<"buyerandid"_n>();

    auto buyers_with_data_id_ids = combine_ids(buyer.value, data->id);
    
    auto exist = buyers_with_data_id.find(buyers_with_data_id_ids);

    check(exist == buyers_with_data_id.end(), "Order from buyer already exist for this data-unit");

    auto id = oorders.available_primary_key();
    
    oorders.emplace(buyer, [&](auto &oo){
      oo.id = id;
      oo.orbdata_id = data_id;
      oo.opened_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      oo.owner = owner;
      oo.buyer = buyer;
      oo.curator = _curator;
      oo.locked_amount = quantity;
      oo.expired_at = eosio::time_point_sec(-1);
    });

      mydataordrs_index mydataordrs (_self, buyer.value);

      auto seller_with_order_id_idx = combine_ids(owner.value, id);

      mydataordrs.emplace(_self, [&](auto &ml){
        ml.owner = owner;
        ml.order_id = id;
      });
 
  }


  void orbapprove_action(const dataapprove &op){
    require_auth(op.username);

    dataorders_index oorders(_self, op.owner.value);
    auto order = oorders.find(op.order_id);
    check(order != oorders.end(), "Order is not found");
    
    orbdata_index orbdts(_self, op.owner.value);
    auto data = orbdts.find(order -> orbdata_id);
    check(data != orbdts.end(), "Data is not found");

    //If this is data-owner, message -> is key.

    if (op.username == order -> owner){
      
      oorders.modify(order, op.username, [&](auto &oo){
        oo.approved = true;
        oo.key = op.message;
        oo.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()+ _DATA_ORDER_EXPIRATION);
      });

    } else if (op.username == order -> buyer){

      check(order -> approved == true, "Cannot finish order before approve from owner");

      action(
            permission_level{ _self, "active"_n },
            data->root_token_contract, "transfer"_n,
            std::make_tuple( _self, order->owner, order -> locked_amount, std::string("Order is closed positive.")) 
      ).send();

      
      orbdts.modify(data, op.username, [&](auto &oo){
        oo.sales_count = data -> sales_count + 1;
      });

      oorders.erase(order);
      
      mydataordrs_index mydataordrs (_self, (order->buyer).value);

      auto seller_with_order_id_idx = combine_ids((order->owner).value, order-> id);

      auto sellers_with_my_order = mydataordrs.find(seller_with_order_id_idx);

      mydataordrs.erase(sellers_with_my_order);
      
      userdatacounts_index usercounts(_self, _self.value);

      auto buyercounts = usercounts.find(op.username.value);
      auto ownercounts = usercounts.find((order-> owner).value);

      if (buyercounts == usercounts.end()){
        
        usercounts.emplace(_self, [&](auto &uc){
          uc.username = op.username;
          uc.total_buys = 1;
        });

      } else {

        usercounts.modify(buyercounts, _self, [&](auto &uc){
          uc.total_buys = buyercounts -> total_buys + 1;
        });

      }

      if (ownercounts == usercounts.end()){
        
        usercounts.emplace(_self, [&](auto &uc){
          uc.username = order-> owner;
          uc.total_sales = 1;
        });

      } else {
        
        usercounts.modify(ownercounts, _self, [&](auto &uc){
          uc.total_sales = ownercounts -> total_sales + 1;
        });

      }

      //TODO add comment to data
      //TODO add likes depends on data
    } else if (op.username == order -> curator){
      //TODO curator cannot do nothing until disput open 

      //TODO transfer tokens to WHO
      //TODO modificate counts
      //
    }

  }

};
