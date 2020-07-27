
    struct [[eosio::table]] comments {
        uint64_t hash;
        uint64_t parent_hash;
        eosio::name author;
        std::string permlink;
        eosio::name parent_author;
        std::string parent_permlink;
        
        std::string body;
        std::string title;
        std::string meta;
        
        eosio::time_point_sec created;
        eosio::time_point_sec last_update;
        
        bool is_encrypted = false;
        std::string public_key;

        bool comments_is_enabled = false;

        int64_t priority = 0;
        
        uint64_t primary_key() const {return hash;}
        
        EOSLIB_SERIALIZE(comments, (hash)(parent_hash)(author)(permlink)(parent_author)(parent_permlink)(body)(title)(meta)(created)(last_update)(is_encrypted)(public_key)(comments_is_enabled)(priority))
    };

    typedef eosio::multi_index<"comments"_n, comments> comments_index;

