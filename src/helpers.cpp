/**
 * @brief      Returns a string representation of an asset.
 *
 * @param[in]  val   The value
 *
 * @return     String representation of the asset.
 */
std::string unicore2::symbol_to_string(eosio::asset val) const
{
    uint64_t v = val.symbol.code().raw();
    v >>= 8;
    string result;
    while (v > 0) {
            char c = static_cast<char>(v & 0xFF);
            result += c;
            v >>= 8;
    }
    return result;
}


uint64_t unicore2::get_global_id(eosio::name key) {

    counts_index counts(_me, _me.value);
    auto count = counts.find(key.value);
    uint64_t id = 1;

    if (count == counts.end()) {
      counts.emplace(_me, [&](auto &c){
        c.key = key;
        c.value = id;
      });
    } else {
      id = count -> value + 1;
      counts.modify(count, _me, [&](auto &c){
        c.value = id;
      });
    }

    return id;

}
