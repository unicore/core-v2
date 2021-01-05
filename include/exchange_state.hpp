#pragma once


#include <eosio/datastream.hpp>
#include <eosio/serialize.hpp>


namespace eosio {
   using eosio::asset;
   using eosio::symbol;

   typedef double real_type;

   /**
    *  Uses Bancor math to create a 50/50 relay between two asset types. The state of the
    *  bancor exchange is entirely contained within this struct. There are no external
    *  side effects associated with using this API.
    */

   struct [[eosio::table, eosio::contract("unicore")]] exchange_state {
      uint64_t id;
      std::string name;
      uint64_t vesting_seconds = 0;
      asset    supply;


      struct connector {
         asset balance;
         double weight = 1;
         eosio::name contract;
         
         EOSLIB_SERIALIZE( connector, (balance)(weight)(contract))
      };

      connector base;
      connector quote;

      uint64_t primary_key()const { return id; }
      
      asset convert_to_exchange( connector& reserve, const asset& payment );
      asset convert_from_exchange( connector& reserve, const asset& tokens );
      asset convert( const asset& from, const symbol& to );

      asset direct_convert( const asset& from, const symbol& to );

      static int64_t get_bancor_output( int64_t inp_reserve,
                                        int64_t out_reserve,
                                        int64_t inp );
      static int64_t get_bancor_input( int64_t out_reserve,
                                       int64_t inp_reserve,
                                       int64_t out );

      EOSLIB_SERIALIZE( exchange_state, (id)(name)(vesting_seconds)(supply)(base)(quote) )
   };

   typedef eosio::multi_index<"powermarket"_n, exchange_state> powermarket;

} /// namespace eosiosystem
