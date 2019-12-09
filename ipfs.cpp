
using namespace eosio;

struct ipfs {

/*
Модуль IPFS
*/

  void setstorage_action(const setstorage &op){
    require_auth(op.username);

    storage_index storages(_self, op.username);
    auto storage = storages.find(op.id);
    
    if (storage == storages.end()){
      
      storages.emplace(op.username, [&](auto &s){
        s.id = op.id;
        s.address = op.address;
      });

    } else {
      storages.modify(storage, op.username, [&](auto &s){
        s.address = op.address;
      });

    };
  }

  void setipfskey_action(const setipfskey &op){
    require_auth(op.username);

    ipfskeys_index ipfskeys(_self, op.username);
    
    ipfskeys.emplace(op.username, [&](auto &i){
      i.id = ipfskeys.available_primary_key();
      i.pkey = op.pkey;
      i.meta = op.meta;
    });


  }


};
