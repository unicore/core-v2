/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;

   class token : public contract {
      public:
         token( eosio::name self ):contract(self){}

         void create( eosio::name issuer,
                      asset        maximum_supply);

         void issue( eosio::name to, asset quantity, string memo );

         void transfer( eosio::name from,
                        eosio::name to,
                        asset        quantity,
                        string       memo );
      
      
         inline asset get_supply( symbol sym )const;
         
         inline asset get_balance( eosio::name owner, symbol sym )const;

      private:
         struct account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.name(); }
         };

         struct currency_stats {
            asset          supply;
            asset          max_supply;
            eosio::name   issuer;

            uint64_t primary_key()const { return supply.symbol.name(); }
         };

         typedef eosio::multi_index<N(accounts), account> accounts;
         typedef eosio::multi_index<N(stat), currency_stats> stats;

         void sub_balance( eosio::name owner, asset value );
         void add_balance( eosio::name owner, asset value, eosio::name ram_payer );

      public:
         struct transfer_args {
            eosio::name  from;
            eosio::name  to;
            asset         quantity;
            string        memo;
         };
   };

   asset token::get_supply( symbol sym )const
   {
      stats statstable( _self, sym );
      const auto& st = statstable.get( sym );
      return st.supply;
   }

   asset token::get_balance( eosio::name owner, symbol sym )const
   {
      accounts accountstable( _self, owner );
      const auto& ac = accountstable.get( sym );
      return ac.balance;
   }

} /// namespace eosio
