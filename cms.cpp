
using namespace eosio;

/**
 * @brief     Модуль CMS
 * Позволяет каждому сообществу использовать веб-конструктор приложений UNI. 
 */

struct cms {

  /**
   * @brief      Метод установки языковых файлов
   *
   * @param[in]  op    The operation
   */
  void setcontent_action(const setcontent &op){
    require_auth(op.username);

    cmscontent_index contents(_self, op.username);
    auto content = contents.find(op.id);
    
    if (content == contents.end()){
      
      contents.emplace(op.username, [&](auto &c){
        c.id = op.id;
        c.lang_id = op.lang_id;
        c.content = op.content;
      });

    } else {
      contents.modify(content, op.username, [&](auto &c){
        c.lang_id = op.lang_id;
        c.content = op.content;
      });
    };
  }

  /**
   * @brief      Метод удаления языкового файла
   *
   * @param[in]  op    The operation
   */
  void rmcontent_action(const rmcontent &op){
    require_auth(op.username);
    cmscontent_index contents(_self, op.username);
    auto content = contents.find(op.id);
    contents.erase(content);
  }


  /**
   * @brief      Метод установки CMS-конфига 
   *
   * @param[in]  op    The operation
   */
  void setcmsconfig_action(const setcmsconfig &op){
    require_auth(op.username);

    cmsconfig_index cmsconfigs(_self, op.username);
    
    account_index accounts(_self, op.username);  

    auto acc = accounts.find(op.username);
    // eosio_assert(acc != accounts.end(), "Host is not found");


    auto config = cmsconfigs.find(1);
    
    if (config == cmsconfigs.end()){
      
      cmsconfigs.emplace(op.username, [&](auto &c){
        c.id = 1;
        c.config = op.config;
      });

    } else {
      cmsconfigs.modify(config, op.username, [&](auto &c){
        c.config = op.config;
      });
    };
  }



};
