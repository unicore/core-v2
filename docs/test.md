xml   Compound {
  name: undefined,
  compounds: {},
  members: [],
  basecompoundref: [],
  filtered: {}
} {
  doxygenindex: {
    '$': {
      'xmlns:xsi': 'http://www.w3.org/2001/XMLSchema-instance',
      'xsi:noNamespaceSchemaLocation': 'index.xsd',
      version: '1.8.14'
    },
    compound: [
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object]
    ]
  }
}
# namespace `eosio`

Класс структур и методов контрактов операционной системы EOSIO.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[``token``](#classeosio_1_1token)    | Класс взаимодействия с жетонами операционной системы.

# class `token` {#classeosio_1_1token}

```
class token
  : public contract
```  

Класс взаимодействия с жетонами операционной системы.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``account``](#structeosio_1_1token_1_1account)        | 
`struct `[``currency_stats``](#structeosio_1_1token_1_1currency__stats)        | 
`public void create(name issuer,asset maximum_supply)` | 
`public void issue(name to,asset quantity,string memo)` | 
`public void retire(asset quantity,string memo)` | 
`public void transfer(name from,name to,asset quantity,string memo)` | 
`public void open(name owner,const symbol & symbol,name ram_payer)` | 
`public void close(name owner,const symbol & symbol)` | 

## Members

### `struct `[``account``](#structeosio_1_1token_1_1account) {#structeosio_1_1token_1_1account}




### `struct `[``currency_stats``](#structeosio_1_1token_1_1currency__stats) {#structeosio_1_1token_1_1currency__stats}




### `public void create(name issuer,asset maximum_supply)` {#classeosio_1_1token_1a398d3ab0784aa52d314ff47666c51701}





### `public void issue(name to,asset quantity,string memo)` {#classeosio_1_1token_1a7dc16dac15ffa8313424676fc02c7015}





### `public void retire(asset quantity,string memo)` {#classeosio_1_1token_1a29c839dfaad1b3c46c2c3ababf232359}





### `public void transfer(name from,name to,asset quantity,string memo)` {#classeosio_1_1token_1a47e89b1fb0cf66946528bdebd9c786d9}





### `public void open(name owner,const symbol & symbol,name ram_payer)` {#classeosio_1_1token_1a119743fdb70ea5f9dc1fea2c7774ed2d}





### `public void close(name owner,const symbol & symbol)` {#classeosio_1_1token_1a35f91e5e77635f0dfadc0eea93ec9da7}






# struct `account` {#structeosio_1_1token_1_1account}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public asset balance` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public asset balance` {#structeosio_1_1token_1_1account_1a6d6aaf37aa4e794b64a1b04990339043}





### `public inline uint64_t primary_key() const` {#structeosio_1_1token_1_1account_1a0881195a3cc88813bfc082903e2e627a}






# struct `currency_stats` {#structeosio_1_1token_1_1currency__stats}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public asset supply` | 
`public asset max_supply` | 
`public name issuer` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public asset supply` {#structeosio_1_1token_1_1currency__stats_1ab24e3519d9ea9063b80d74a64ec1fd73}





### `public asset max_supply` {#structeosio_1_1token_1_1currency__stats_1ab83559b6e1cdd215cf7b60153009a966}





### `public name issuer` {#structeosio_1_1token_1_1currency__stats_1a98c87418ed2e156ca2bd94a8e51a0b9f}





### `public inline uint64_t primary_key() const` {#structeosio_1_1token_1_1currency__stats_1ac651a49171c9e1b62d07e529657895ec}






# namespace `eosiosystem`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------

# class `unicore` {#classunicore}

```
class unicore
  : public contract
```  

Содержит все доступные действия, публичные и приватные методы протокола.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public inline  unicore(eosio::name receiver,eosio::name code,eosio::datastream< const char *> ds)` | 
`public void apply(uint64_t receiver,uint64_t code,uint64_t action)` | 
`public void convert(eosio::name username,eosio::name host,uint64_t balance_id)` | Публичный метод обмена баланса на жетон распределения по текущему курсу из числа квантов раунда.
`public void setparams(eosio::name host,eosio::name chost,uint64_t size_of_pool,uint64_t quants_precision,uint64_t overlap,uint64_t profit_growth,uint64_t base_rate,uint64_t loss_percent,uint64_t compensator_percent,uint64_t pool_limit,uint64_t pool_timeout,uint64_t priority_seconds)` | Публичный метод установки параметров протокола двойной спирали Вызывается пользователем после базовой операции создания хоста и проведения оплаты. Так же вызывается при установке параметров дочернего хоста. Содержит алгоритм финансового ядра. Производит основные расчеты таблиц курсов и валидирует положительность бизнес-дохода.
`public void start(eosio::name host,eosio::name chost)` | Публичный метод запуска хоста Метод необходимо вызвать для запуска хоста после установки параметров хоста. Добавляет первый цикл, два пула, переключает демонастративный флаг запуска и создает статистические объекты. Подписывается аккаунтом хоста.
`public void withdraw(eosio::name username,eosio::name host,uint64_t balance_id)` | Публичный метод возврата баланса протоколу. Вывод средств возможен только для полностью обновленных (актуальных) балансов. Производит обмен Юнитов на управляющий токен и выплачивает на аккаунт пользователя.
`public void priorenter(eosio::name username,eosio::name host,uint64_t balance_id)` | Метод приоритетного входа в новый цикл. Доступен к использованию только при условии наличия предыдущего цикла, в котором участник имеет проигравший баланс. Позволяет зайти частью своего проигравшего баланса одновременно в два пула - первый и второй нового цикла. Использование приоритетного входа возможно только до истечения времени приоритета, которое начинается в момент запуска цикла и продолжается до истечения таймера приоритета.
`public void refreshbal(eosio::name username,uint64_t balance_id,uint64_t partrefresh)` | Публичный метод обновления баланса Пересчет баланса каждого пользователя происходит по его собственному действию. Обновление баланса приводит к пересчету доступной суммы для вывода.
`public void setstartdate(eosio::name host,eosio::time_point_sec start_at)` | 
`public void init(uint64_t system_income)` | 
`public void refreshst(eosio::name host)` | Публичный метод обновления состояния Проверяет пул на истечение во времени или завершение целого количества ядерных Юнитов. Запускает новый цикл или добавляет новый пул.
`public void setbadge(uint64_t id,eosio::name host,eosio::string caption,eosio::string description,eosio::string iurl,eosio::string pic,uint64_t total,uint64_t power)` | 
`public void giftbadge(eosio::name host,eosio::name to,uint64_t badge_id,eosio::string comment,bool netted,uint64_t goal_id,uint64_t task_id)` | 
`public void backbadge(eosio::name host,eosio::name from,uint64_t usbadge_id,eosio::string comment)` | Метод возврата значка Может быть использован хостом для возврата выданного значка.
`public void setcontent(eosio::name username,eosio::name type,eosio::name lang,eosio::string content)` | Модуль CMS Позволяет каждому сообществу использовать веб-конструктор приложений UNI.
`public void rmcontent(eosio::name username,eosio::name type)` | Метод удаления языкового файла
`public void setcmsconfig(eosio::name username,eosio::string config)` | Метод установки CMS-конфига
`public void setgoal(eosio::name creator,eosio::name host,eosio::name type,std::string title,std::string permlink,std::string description,eosio::asset target,uint64_t duration,uint64_t cashback,bool activated,bool is_batch,uint64_t parent_batch_id,std::string meta)` | Метод создания цели
`public void dfundgoal(eosio::name architect,eosio::name host,uint64_t goal_id,eosio::asset amount,std::string comment)` | Метод прямого финансирования цели Используется архитектором для финансирования цели из фонда
`public void fundchildgoa(eosio::name architect,eosio::name host,uint64_t goal_id,eosio::asset amount)` | Метод прямого финансирования родительской цели Используется архитектором для наполнения дочерней цели за счет баланса родительской цели
`public void setgcreator(eosio::name oldcreator,eosio::name newcreator,eosio::name host,uint64_t goal_id)` | 
`public void gaccept(eosio::name host,uint64_t goal_id,uint64_t parent_goal_id,bool status)` | 
`public void gpause(eosio::name host,uint64_t goal_id)` | 
`public void delgoal(eosio::name username,eosio::name host,uint64_t goal_id)` | Метод удаления цели
`public void report(eosio::name username,eosio::name host,uint64_t goal_id,std::string report)` | Метод создания отчета о завершенной цели
`public void check(eosio::name architect,eosio::name host,uint64_t goal_id)` | Метод проверки отчета Отчет о достижении цели на текущий момент проверяется только архитектором.
`public void gwithdraw(eosio::name username,eosio::name host,uint64_t goal_id)` | Метод вывода баланса цели Выводит доступный баланс цели на аккаунт создателя цели.
`public void gsponsor(eosio::name hoperator,eosio::name host,eosio::name reciever,uint64_t goal_id,eosio::asset amount)` | Метод финансирования цели через оператора сообщества. Позволяет оператору сообщества расходовать баланс целей на перечисления прямым спонсорам. Используется в риверсной экономической модели, когда корневой токен сообщества является котировочным токеном силы сообщества, и накаким другим способом изначально не распределяется, кроме как на спонсоров целей (дефицитная ICO - модель).
`public void setemi(eosio::name host,uint64_t percent,uint64_t gtop)` | Метод установки скорости эмиссии и размера листа финансирования
`public void enablesale(eosio::name host,eosio::name token_contract,eosio::asset asset_on_sale,int64_t sale_shift,eosio::name sale_mode)` | Публичный метод включения сейла с хоста. Может быть использован только до вызова метода start при условии, что владелец контракта токена разрешил это. Активирует реализацию управляющего жетона из фонда владельца жетона в режиме финансового котла.
`public void addhostofund(uint64_t fund_id,eosio::name host)` | Публичный метод подключения хоста к фонду
`public void rmhosfrfund(uint64_t fund_id,eosio::name host)` | 
`public void transfromgf(eosio::name to,eosio::name token_contract,eosio::asset quantity)` | Публичный метод перевода из глобального фонда
`public void setarch(eosio::name host,eosio::name architect)` | Sets the architect. Устанавливает архитектора сообщества, обладающего полномочиями специальных действий.
`public void upgrade(eosio::name username,eosio::name platform,std::string title,std::string purpose,uint64_t total_shares,eosio::asset quote_amount,eosio::name quote_token_contract,eosio::asset root_token,eosio::name root_token_contract,bool voting_only_up,uint64_t consensus_percent,uint64_t referral_percent,uint64_t dacs_percent,uint64_t cfund_percent,uint64_t hfund_percent,std::vector< uint64_t > levels,uint64_t emission_percent,uint64_t gtop,std::string meta)` | Метод апгрейда аккаунта до статуса сообщества Принимает ряд параметров, такие как процент консенсуса, реферальный процент, уровни вознаграждений финансовых партнеров, корпоративный процент, а так же параметры рынка силы.
`public void cchildhost(eosio::name parent_host,eosio::name chost)` | Метод создания дочернего хоста Позволяет сообществу на границе циклов изменять параметры финансового ядра, распологая их в области памяти аккаунта дочернего хоста.
`public void compensator(eosio::name host,uint64_t compensator_percent)` | Установка компенсатора рисков Устанавливает компенсатор риска для системного жетона
`public void edithost(eosio::name architect,eosio::name host,eosio::name platform,eosio::string title,eosio::string purpose,eosio::string manifest,eosio::name power_market_id,eosio::string meta)` | Метод редактирования информации о хосте
`public void fixs(eosio::name host,uint64_t pool_num)` | 
`public void settype(eosio::name host,eosio::name type)` | 
`public void setlevels(eosio::name host,std::vector< uint64_t > levels)` | Метод установки уровней вознаграждений
`public void settiming(eosio::name host,uint64_t pool_timeout,uint64_t priority_seconds)` | Метод редактирования времени
`public void setflows(eosio::name host,uint64_t ref_percent,uint64_t dacs_percent,uint64_t cfund_percent,uint64_t hfund_percent)` | 
`public void rmben(eosio::name creator,eosio::name username,eosio::name host,uint64_t goal_id)` | 
`public void addben(eosio::name creator,eosio::name username,eosio::name host,uint64_t goal_id,uint64_t weight)` | 
`public void withdrbeninc(eosio::name username,eosio::name host,uint64_t goal_id)` | 
`public void addvac(uint64_t id,bool is_edit,eosio::name creator,eosio::name host,eosio::name limit_type,eosio::asset income_limit,uint64_t weight,std::string title,std::string descriptor)` | 
`public void rmvac(eosio::name host,uint64_t id)` | 
`public void addvprop(uint64_t id,bool is_edit,eosio::name creator,eosio::name host,uint64_t vac_id,uint64_t weight,std::string why_me,std::string contacts)` | 
`public void rmvprop(eosio::name host,uint64_t vprop_id)` | 
`public void approvevac(eosio::name host,uint64_t vac_id)` | 
`public void apprvprop(eosio::name host,uint64_t vprop_id)` | 
`public void adddac(eosio::name username,eosio::name host,uint64_t weight,eosio::name limit_type,eosio::asset income_limit,std::string title,std::string descriptor)` | 
`public void rmdac(eosio::name username,eosio::name host)` | 
`public void suggestrole(eosio::name username,std::string title,std::string descriptor)` | 
`public void withdrdacinc(eosio::name username,eosio::name host)` | 
`public void setwebsite(eosio::name host,eosio::name ahostname,eosio::string website,eosio::name type,std::string meta)` | 
`public void rmahost(eosio::name host,eosio::name ahostname)` | 
`public void setahost(eosio::name host,eosio::name ahostname)` | 
`public void closeahost(eosio::name host)` | 
`public void openahost(eosio::name host)` | 
`public void withpbenefit(eosio::name username,eosio::name host)` | Метод вывода силового финансового потока withdraw power quant (withpowerun) Позволяет вывести часть финансового потока, направленного на держателя силы
`public void withrsegment(eosio::name username,eosio::name host)` | Метод вывода остатка партнерского финансового потока withdraw power quant (withpowerun)
`public void withrbenefit(eosio::name username,eosio::name host,uint64_t id)` | Метод вывода партнерского финансового потока withdraw power quant (withpowerun)
`public void refreshpu(eosio::name username,eosio::name host)` | Метод обновления силового финансового потока
`public void refreshsh(eosio::name owner,uint64_t id)` | Метод обновления вестинг-баланса. <br/>Обновляет вестинг-баланс до актуальных параметров продолжительности.
`public void withdrawsh(eosio::name owner,uint64_t id)` | Вывод вестинг-баланса Обеспечивает вывод доступных средств из вестинг-баланса.
`public void sellshares(eosio::name username,eosio::name host,uint64_t shares)` | Публичный метод продажи силы рынку за котировочный токен
`public void undelshares(eosio::name from,eosio::name reciever,eosio::name host,uint64_t shares)` | Метод возврата делегированной силы
`public void settask(eosio::name host,eosio::name creator,std::string permlink,uint64_t goal_id,uint64_t priority,eosio::string title,eosio::string data,eosio::asset requested,bool is_public,eosio::name doer,eosio::asset for_each,bool with_badge,uint64_t badge_id,uint64_t duration,bool is_batch,uint64_t parent_batch_id,bool is_regular,std::vector< uint64_t > calendar,eosio::time_point_sec start_at,eosio::time_point_sec expired_at,std::string meta)` | Модуль задач Задачи есть составляющие части достижения любой цели. Постановка задач осуществляется только в рамках целей. Задачи могут быть
`public void fundtask(eosio::name host,uint64_t task_id,eosio::asset amount,eosio::string comment)` | Публичный метод фондирования задачи Исполняется хостом для пополнения баланса задачи из доступного баланса цели.
`public void tactivate(eosio::name host,uint64_t task_id)` | Метод активации задачи Вызывается хостом для активации выполнения поставленной задачи.
`public void tdeactivate(eosio::name host,uint64_t task_id)` | Публичный метод деактивации задачи Применимо для публичных задач, когда поставленная цель достигнута или недостижима.
`public void tcomplete(eosio::name host,uint64_t task_id)` | Публичный метод деактивации задачи Применимо для публичных задач, когда поставленная цель достигнута или недостижима.
`public void setpgoal(eosio::name host,uint64_t task_id,uint64_t goal_id)` | Метод установки родительской цели
`public void setdoer(eosio::name host,uint64_t task_id,eosio::name doer)` | Метод установки исполнителя
`public void validate(eosio::name host,uint64_t task_id,uint64_t goal_id,eosio::name doer)` | Метод ручной валидации задачи
`public void jointask(eosio::name host,uint64_t task_id,eosio::name doer,std::string comment)` | Метод предложения себя как исполнителя задачи
`public void canceljtask(eosio::name host,uint64_t task_id,eosio::name doer)` | Метод предложения отклонения заявки деятеля
`public void settaskmeta(eosio::name host,uint64_t task_id,std::string meta)` | Метод ручной валидации задачи
`public void setinctask(eosio::name username,uint64_t income_id,bool with_badge,uint64_t my_goal_id,uint64_t my_badge_id)` | 
`public void deltask(eosio::name host,uint64_t task_id)` | Метод удаления задачи Вызывается хостом для удаления поставленной задачи.
`public void paydebt(eosio::name host,uint64_t goal_id)` | Метод выплаты долга по цели из числа available в счет объектов долга
`public void setreport(eosio::name host,eosio::name username,uint64_t task_id,eosio::string data)` | Публичный метод создания отчета о выполненной задаче Применяется исполнителем задачи для того, чтобы отправить отчет на проверку.
`public void editreport(eosio::name host,eosio::name username,uint64_t report_id,eosio::string data)` | Публиный метод редактирования отчета В случае, если отчет не принят, участник получает возможность отредактировать свой отчет и выслать его на проверку повторно.
`public void approver(eosio::name host,uint64_t report_id,eosio::string comment)` | Публичный метод одобрения отчета Используется хостом для того, чтобы принять задачу как выполненную и выдать вознаграждение / награду в виде значка.
`public void disapprover(eosio::name host,uint64_t report_id,eosio::string comment)` | Публичный метод отклонения отчета
`public void withdrawrepo(eosio::name username,eosio::name host,uint64_t report_id)` | Публичный метод вывода баланса отчёта
`public void distrepo(eosio::name host,uint64_t report_id)` | Публичный метод обновления баланса отчёта при распределении баланса цели
`public void vote(eosio::name voter,eosio::name host,uint64_t goal_id,bool up)` | 
`public void rvote(eosio::name voter,eosio::name host,uint64_t report_id,bool up)` | 
`public void setcondition(eosio::name host,eosio::string key,uint64_t value)` | 
`public void rmcondition(eosio::name host,uint64_t key)` | 
`public std::string symbol_to_string(eosio::asset val) const` | Структура хоста

## Members

### `public inline  unicore(eosio::name receiver,eosio::name code,eosio::datastream< const char *> ds)` {#classunicore_1a8f1c99385890a2fe4645c4af8e7fcfc7}





### `public void apply(uint64_t receiver,uint64_t code,uint64_t action)` {#classunicore_1added73f623a6df0537a4daef1c46ed8b}





### `public void convert(eosio::name username,eosio::name host,uint64_t balance_id)` {#classunicore_1a30206473eb9672b7b67d0a85d2035ff3}

Публичный метод обмена баланса на жетон распределения по текущему курсу из числа квантов раунда.



### `public void setparams(eosio::name host,eosio::name chost,uint64_t size_of_pool,uint64_t quants_precision,uint64_t overlap,uint64_t profit_growth,uint64_t base_rate,uint64_t loss_percent,uint64_t compensator_percent,uint64_t pool_limit,uint64_t pool_timeout,uint64_t priority_seconds)` {#classunicore_1a8898e75bc9abe84fd78c15f9de3731f5}

Публичный метод установки параметров протокола двойной спирали Вызывается пользователем после базовой операции создания хоста и проведения оплаты. Так же вызывается при установке параметров дочернего хоста. Содержит алгоритм финансового ядра. Производит основные расчеты таблиц курсов и валидирует положительность бизнес-дохода.

#### Parameters
* `op` The operation

### `public void start(eosio::name host,eosio::name chost)` {#classunicore_1ad83b75f3c4afd1ab954d428be8f6f7d7}

Публичный метод запуска хоста Метод необходимо вызвать для запуска хоста после установки параметров хоста. Добавляет первый цикл, два пула, переключает демонастративный флаг запуска и создает статистические объекты. Подписывается аккаунтом хоста.

#### Parameters
* `op` The operation

### `public void withdraw(eosio::name username,eosio::name host,uint64_t balance_id)` {#classunicore_1a25c98e5ea071940b6ab153d71a328a5c}

Публичный метод возврата баланса протоколу. Вывод средств возможен только для полностью обновленных (актуальных) балансов. Производит обмен Юнитов на управляющий токен и выплачивает на аккаунт пользователя.

Производит расчет реферальных вознаграждений, генерируемых выплатой, и отправляет их всем партнерам согласно установленной формы.

Производит финансовое распределение между управляющими компаниями и целевым фондом сообщества.

Каждый последующий пул, который участник проходит в качестве победителя, генеририрует бизнес-доход, который расчитывается исходя из того, что в текущий момент, средств всех проигравших полностью достаточно наа выплаты всем победителям с остатком. Этот остаток, в прапорции Юнитов пользователя и общего количества Юнитов в раунде, позволяет расчитать моментальную выплату, которая может быть изъята из системы при сохранении абсолютного балланса.

Изымаемая сумма из общего котла управляющих токенов, разделяется на три потока, определяемые двумя параметрами:

* Процент выплат на рефералов. Устанавливается в диапазоне от 0 до 100. Отсекает собой ровно ту часть пирога, которая уходит на выплаты по всем уровням реферальной структуры согласно ее формы.


* Процент выплат на корпоративных управляющих. Устанавливается в диапазоне от 0 до 100.


* Остаток от остатка распределяется в фонд целей сообщества.





Таким образом, коэффициенты позволяют распределять единый системный фонд по окружности с тремя секторами, где каждый сектор есть фонд со своим назначением. 
 Например, если общий доход от движения баланса пользователя по спирали составляет 100 USD, а коэфициенты распределения Рефералов и Корпоративных Управляющих равняются соответственно по 0.5 (50%), то все рефералы получат по 33$, все управляющие получат по 33$, и еще 33$ попадет в фонд целей сообщества. 1$ останется в качестве комиссии округления на делении у протокола.


#### Parameters
* `op` The operation

### `public void priorenter(eosio::name username,eosio::name host,uint64_t balance_id)` {#classunicore_1a7c554fbaa488514c2fd09879a6b4c662}

Метод приоритетного входа в новый цикл. Доступен к использованию только при условии наличия предыдущего цикла, в котором участник имеет проигравший баланс. Позволяет зайти частью своего проигравшего баланса одновременно в два пула - первый и второй нового цикла. Использование приоритетного входа возможно только до истечения времени приоритета, которое начинается в момент запуска цикла и продолжается до истечения таймера приоритета.

Метод позволяет проигравшим балансам предыдущего цикла перераспределиться в новом цикле развития так, чтобы быть в самом начале вращательного движения и тем самым, гарантировать выигрыш. В случае успеха исполнения метода, пользователь обменивает один свой старый проигравший баланс на два новых противоположных цветов.

В ходе исполнения метода решается арифметическая задача перераспределения, при которой вычисляется максимально возможная сумма входа для имеющегося баланса одновременно в два первых пула. Остаток от суммы, который невозможно распределить на новые пулы по причине нераздельности минимальной учетной единицы, возвращается пользователю переводом.

Приоритетный вход спроектирован таким образом, то если все проигравшие участники одновременно воспользуются им, то в точности 50% внутренней учетной единицы для первого и второго пула будет выкуплено.

TODO возможно расширение приоритетного входа до 100% от внутренней учетной единицы для первых двух пулов, а так же, продолжение приоритетного входа для всех последующих пулов.


#### Parameters
* `op` The operation

### `public void refreshbal(eosio::name username,uint64_t balance_id,uint64_t partrefresh)` {#classunicore_1abb1a56452f7d8819475e04e5ad10fe54}

Публичный метод обновления баланса Пересчет баланса каждого пользователя происходит по его собственному действию. Обновление баланса приводит к пересчету доступной суммы для вывода.

#### Parameters
* `op` The operation

### `public void setstartdate(eosio::name host,eosio::time_point_sec start_at)` {#classunicore_1a62e02b155f2cd8012d807e60dacd1ace}





### `public void init(uint64_t system_income)` {#classunicore_1a43103f3b9bdc5ed37f564cc10446549e}





### `public void refreshst(eosio::name host)` {#classunicore_1a2a410729955a5bc11fe3b3051f10a4c9}

Публичный метод обновления состояния Проверяет пул на истечение во времени или завершение целого количества ядерных Юнитов. Запускает новый цикл или добавляет новый пул.

//TODO устранить избыточность


#### Parameters
* `host` The host

### `public void setbadge(uint64_t id,eosio::name host,eosio::string caption,eosio::string description,eosio::string iurl,eosio::string pic,uint64_t total,uint64_t power)` {#classunicore_1a46cd90e0a4f1b938d4548ca254782cfe}





### `public void giftbadge(eosio::name host,eosio::name to,uint64_t badge_id,eosio::string comment,bool netted,uint64_t goal_id,uint64_t task_id)` {#classunicore_1a40fa6e2fa9fcddf76ba73b261eae6fef}





### `public void backbadge(eosio::name host,eosio::name from,uint64_t usbadge_id,eosio::string comment)` {#classunicore_1a6e7db779eb5836300fabe105012388c2}

Метод возврата значка Может быть использован хостом для возврата выданного значка.

#### Parameters
* `op` The operation

### `public void setcontent(eosio::name username,eosio::name type,eosio::name lang,eosio::string content)` {#classunicore_1abab6ddd4a167efde5f0e45a1ffd9dfba}

Модуль CMS Позволяет каждому сообществу использовать веб-конструктор приложений UNI.

Метод установки языковых файлов


#### Parameters
* `op` The operation

### `public void rmcontent(eosio::name username,eosio::name type)` {#classunicore_1af6aac321d5880fcb577ba00abea5c38f}

Метод удаления языкового файла

#### Parameters
* `op` The operation

### `public void setcmsconfig(eosio::name username,eosio::string config)` {#classunicore_1ad0188feae42e22b52afbb6e3c3f70c86}

Метод установки CMS-конфига

#### Parameters
* `op` The operation

### `public void setgoal(eosio::name creator,eosio::name host,eosio::name type,std::string title,std::string permlink,std::string description,eosio::asset target,uint64_t duration,uint64_t cashback,bool activated,bool is_batch,uint64_t parent_batch_id,std::string meta)` {#classunicore_1aa2341c3f2393be329d759ef09f20367f}

Метод создания цели

#### Parameters
* `op` The new value

### `public void dfundgoal(eosio::name architect,eosio::name host,uint64_t goal_id,eosio::asset amount,std::string comment)` {#classunicore_1a969e75dd90699b53b75ab8482cbc49d3}

Метод прямого финансирования цели Используется архитектором для финансирования цели из фонда

#### Parameters
* `op` The operation

### `public void fundchildgoa(eosio::name architect,eosio::name host,uint64_t goal_id,eosio::asset amount)` {#classunicore_1af6e32936bbe20a612c9348bf5f805982}

Метод прямого финансирования родительской цели Используется архитектором для наполнения дочерней цели за счет баланса родительской цели

#### Parameters
* `op` The operation

### `public void setgcreator(eosio::name oldcreator,eosio::name newcreator,eosio::name host,uint64_t goal_id)` {#classunicore_1a1ba1753b7dc4109e94dc7c4c9dbd5400}





### `public void gaccept(eosio::name host,uint64_t goal_id,uint64_t parent_goal_id,bool status)` {#classunicore_1a2ee6a26c970b05e61c3753791a55dee4}





### `public void gpause(eosio::name host,uint64_t goal_id)` {#classunicore_1a2cc7c4ae8c6ea5e604d6666d37d11405}





### `public void delgoal(eosio::name username,eosio::name host,uint64_t goal_id)` {#classunicore_1a9fd534e0b189439c6f18e99689911b3e}

Метод удаления цели

#### Parameters
* `op` The operation

### `public void report(eosio::name username,eosio::name host,uint64_t goal_id,std::string report)` {#classunicore_1ac0e6bf94b8bfd28e08c364631a357d91}

Метод создания отчета о завершенной цели

#### Parameters
* `op` The operation

### `public void check(eosio::name architect,eosio::name host,uint64_t goal_id)` {#classunicore_1ac31cf68445b14c4dbdea9684a0556f10}

Метод проверки отчета Отчет о достижении цели на текущий момент проверяется только архитектором.

#### Parameters
* `op` The operation

### `public void gwithdraw(eosio::name username,eosio::name host,uint64_t goal_id)` {#classunicore_1a4faa41a68505840e66f78e350412723c}

Метод вывода баланса цели Выводит доступный баланс цели на аккаунт создателя цели.

#### Parameters
* `op` The operation

### `public void gsponsor(eosio::name hoperator,eosio::name host,eosio::name reciever,uint64_t goal_id,eosio::asset amount)` {#classunicore_1a621817942c7b2758963d4147170b3c60}

Метод финансирования цели через оператора сообщества. Позволяет оператору сообщества расходовать баланс целей на перечисления прямым спонсорам. Используется в риверсной экономической модели, когда корневой токен сообщества является котировочным токеном силы сообщества, и накаким другим способом изначально не распределяется, кроме как на спонсоров целей (дефицитная ICO - модель).

#### Parameters
* `op` The operation

### `public void setemi(eosio::name host,uint64_t percent,uint64_t gtop)` {#classunicore_1affa39993f35c8e2c8838a956c761cbcc}

Метод установки скорости эмиссии и размера листа финансирования

#### Parameters
* `op` The new value

### `public void enablesale(eosio::name host,eosio::name token_contract,eosio::asset asset_on_sale,int64_t sale_shift,eosio::name sale_mode)` {#classunicore_1ad1ca7dd3eab26146ed184e3bda67f5b1}

Публичный метод включения сейла с хоста. Может быть использован только до вызова метода start при условии, что владелец контракта токена разрешил это. Активирует реализацию управляющего жетона из фонда владельца жетона в режиме финансового котла.



### `public void addhostofund(uint64_t fund_id,eosio::name host)` {#classunicore_1a2b873bf336e3c8b5b6e0f9d1823b2497}

Публичный метод подключения хоста к фонду



### `public void rmhosfrfund(uint64_t fund_id,eosio::name host)` {#classunicore_1a8474a6b0859f13d7283922d5f7c3cd0a}





### `public void transfromgf(eosio::name to,eosio::name token_contract,eosio::asset quantity)` {#classunicore_1a69785f67b90bf9b74e5a6c177cb0e249}

Публичный метод перевода из глобального фонда



### `public void setarch(eosio::name host,eosio::name architect)` {#classunicore_1ac5b6ba6e6362cc73943265c750429fd4}

Sets the architect. Устанавливает архитектора сообщества, обладающего полномочиями специальных действий.

#### Parameters
* `op` The new value

### `public void upgrade(eosio::name username,eosio::name platform,std::string title,std::string purpose,uint64_t total_shares,eosio::asset quote_amount,eosio::name quote_token_contract,eosio::asset root_token,eosio::name root_token_contract,bool voting_only_up,uint64_t consensus_percent,uint64_t referral_percent,uint64_t dacs_percent,uint64_t cfund_percent,uint64_t hfund_percent,std::vector< uint64_t > levels,uint64_t emission_percent,uint64_t gtop,std::string meta)` {#classunicore_1adabf2b3d106784b146a047e35ceec410}

Метод апгрейда аккаунта до статуса сообщества Принимает ряд параметров, такие как процент консенсуса, реферальный процент, уровни вознаграждений финансовых партнеров, корпоративный процент, а так же параметры рынка силы.

#### Parameters
* `op` The operation

### `public void cchildhost(eosio::name parent_host,eosio::name chost)` {#classunicore_1abae3b92d93cbbbd1058005639975f09d}

Метод создания дочернего хоста Позволяет сообществу на границе циклов изменять параметры финансового ядра, распологая их в области памяти аккаунта дочернего хоста.

#### Parameters
* `op` The operation

### `public void compensator(eosio::name host,uint64_t compensator_percent)` {#classunicore_1abb38bcc39c9e3ca74f955944a84274fe}

Установка компенсатора рисков Устанавливает компенсатор риска для системного жетона

#### Parameters
* `op` The new value

### `public void edithost(eosio::name architect,eosio::name host,eosio::name platform,eosio::string title,eosio::string purpose,eosio::string manifest,eosio::name power_market_id,eosio::string meta)` {#classunicore_1a38d070fb8067b91799456f1472ced47d}

Метод редактирования информации о хосте

#### Parameters
* `op` The operation

### `public void fixs(eosio::name host,uint64_t pool_num)` {#classunicore_1abb22debda627e37951ea15e47fc77985}





### `public void settype(eosio::name host,eosio::name type)` {#classunicore_1a61965d7a5c449487b565b444659d6cd7}





### `public void setlevels(eosio::name host,std::vector< uint64_t > levels)` {#classunicore_1a9ebdfe87e84d1a283143d274f2337e84}

Метод установки уровней вознаграждений

#### Parameters
* `op` The operation

### `public void settiming(eosio::name host,uint64_t pool_timeout,uint64_t priority_seconds)` {#classunicore_1af4454491cb629dc007eed9b6febdd161}

Метод редактирования времени

#### Parameters
* `op` The operation

### `public void setflows(eosio::name host,uint64_t ref_percent,uint64_t dacs_percent,uint64_t cfund_percent,uint64_t hfund_percent)` {#classunicore_1a2b63d1aac35b533f122286943a54db18}





### `public void rmben(eosio::name creator,eosio::name username,eosio::name host,uint64_t goal_id)` {#classunicore_1a1713b099515838ad2e42391f4dd03b6f}





### `public void addben(eosio::name creator,eosio::name username,eosio::name host,uint64_t goal_id,uint64_t weight)` {#classunicore_1a4c8df6c80a48d2a6f8a73469a470ec2f}





### `public void withdrbeninc(eosio::name username,eosio::name host,uint64_t goal_id)` {#classunicore_1a0a2ae5cf5fb3cdf3ba23f0948dd5c391}





### `public void addvac(uint64_t id,bool is_edit,eosio::name creator,eosio::name host,eosio::name limit_type,eosio::asset income_limit,uint64_t weight,std::string title,std::string descriptor)` {#classunicore_1aabd42ff2a6dcac81d487e65b0bf5dda6}





### `public void rmvac(eosio::name host,uint64_t id)` {#classunicore_1a756863a6910963e79adbb3bfcdf7ba47}





### `public void addvprop(uint64_t id,bool is_edit,eosio::name creator,eosio::name host,uint64_t vac_id,uint64_t weight,std::string why_me,std::string contacts)` {#classunicore_1a728bf4c6d03c63fa2969a3effe312fea}





### `public void rmvprop(eosio::name host,uint64_t vprop_id)` {#classunicore_1adb6489dbbbb032b012f3faa9e8ba963f}





### `public void approvevac(eosio::name host,uint64_t vac_id)` {#classunicore_1a55e754af41251b55a9c6b1cfe0f0f8c1}





### `public void apprvprop(eosio::name host,uint64_t vprop_id)` {#classunicore_1afcacb23342b138f05740e3bd57d03fde}





### `public void adddac(eosio::name username,eosio::name host,uint64_t weight,eosio::name limit_type,eosio::asset income_limit,std::string title,std::string descriptor)` {#classunicore_1ac6f84b754aa586043376617165f90eca}





### `public void rmdac(eosio::name username,eosio::name host)` {#classunicore_1a1a76e2539a42f80b8ac6f514305665c8}





### `public void suggestrole(eosio::name username,std::string title,std::string descriptor)` {#classunicore_1ab006e60df2a0c5bcac711ca458e5cbc4}





### `public void withdrdacinc(eosio::name username,eosio::name host)` {#classunicore_1a00bbf7a3172cc73697b3fcdfc19b7a4b}





### `public void setwebsite(eosio::name host,eosio::name ahostname,eosio::string website,eosio::name type,std::string meta)` {#classunicore_1a3a1fde814fc862b339230cfb85505145}





### `public void rmahost(eosio::name host,eosio::name ahostname)` {#classunicore_1ab1e9b13cec99a231f1c6a71e3edc37c6}





### `public void setahost(eosio::name host,eosio::name ahostname)` {#classunicore_1a1f73391b2c692e2c97ca675333aee0ad}





### `public void closeahost(eosio::name host)` {#classunicore_1abf1dd4d893faaa4851adced7cfa7770c}





### `public void openahost(eosio::name host)` {#classunicore_1a339ec2fd85cf7ddd182a7e41d0f66f66}





### `public void withpbenefit(eosio::name username,eosio::name host)` {#classunicore_1a78e7d25bcf723965ab405b66327a23e7}

Метод вывода силового финансового потока withdraw power quant (withpowerun) Позволяет вывести часть финансового потока, направленного на держателя силы



### `public void withrsegment(eosio::name username,eosio::name host)` {#classunicore_1ad330ad73fc269d1e37df00b7e9823441}

Метод вывода остатка партнерского финансового потока withdraw power quant (withpowerun)



### `public void withrbenefit(eosio::name username,eosio::name host,uint64_t id)` {#classunicore_1ad0eeced7a1cf17a871f0458eb327da6e}

Метод вывода партнерского финансового потока withdraw power quant (withpowerun)



### `public void refreshpu(eosio::name username,eosio::name host)` {#classunicore_1a00ca80d8fa12bd41448d55cd82c5cd60}

Метод обновления силового финансового потока

Позволяет обновить доступную часть финансового потока, направленного на держателя силы, а так же собрать доступные реферальные балансы в один объект

### `public void refreshsh(eosio::name owner,uint64_t id)` {#classunicore_1a790c1a22331df2bd9e7f9b9094de7dfb}

Метод обновления вестинг-баланса. 
Обновляет вестинг-баланс до актуальных параметров продолжительности.

#### Parameters
* `op` The operation

### `public void withdrawsh(eosio::name owner,uint64_t id)` {#classunicore_1a4dd6d244c3fac428419342dc782f53a1}

Вывод вестинг-баланса Обеспечивает вывод доступных средств из вестинг-баланса.

#### Parameters
* `op` The operation

### `public void sellshares(eosio::name username,eosio::name host,uint64_t shares)` {#classunicore_1a237ab7bb223b33ba87375e177ced89a2}

Публичный метод продажи силы рынку за котировочный токен

#### Parameters
* `op` The operation

### `public void undelshares(eosio::name from,eosio::name reciever,eosio::name host,uint64_t shares)` {#classunicore_1a9464479eb5d28b6c7f9956319077b294}

Метод возврата делегированной силы

#### Parameters
* `op` The operation

### `public void settask(eosio::name host,eosio::name creator,std::string permlink,uint64_t goal_id,uint64_t priority,eosio::string title,eosio::string data,eosio::asset requested,bool is_public,eosio::name doer,eosio::asset for_each,bool with_badge,uint64_t badge_id,uint64_t duration,bool is_batch,uint64_t parent_batch_id,bool is_regular,std::vector< uint64_t > calendar,eosio::time_point_sec start_at,eosio::time_point_sec expired_at,std::string meta)` {#classunicore_1aa989f60384b7167e4e1ff2b4d219a1b3}

Модуль задач Задачи есть составляющие части достижения любой цели. Постановка задач осуществляется только в рамках целей. Задачи могут быть

* публичными, т.е. достуными к исполнению каждым участником сообщества,


* приватными, т.е. доступными к исполнению только внутренним управляющим командам. Публичный метод создания задачи Может использовать только аккаунт хоста на текущий момент.






#### Parameters
* `op` The operation

### `public void fundtask(eosio::name host,uint64_t task_id,eosio::asset amount,eosio::string comment)` {#classunicore_1ae8ca388e55f11d3c32430edbaabcf7e2}

Публичный метод фондирования задачи Исполняется хостом для пополнения баланса задачи из доступного баланса цели.

#### Parameters
* `op` The operation

### `public void tactivate(eosio::name host,uint64_t task_id)` {#classunicore_1a0c35d9830c0c05c8f13be48b8cbdf236}

Метод активации задачи Вызывается хостом для активации выполнения поставленной задачи.

#### Parameters
* `op` The operation

### `public void tdeactivate(eosio::name host,uint64_t task_id)` {#classunicore_1a725477908310816ec80cddbf733af04b}

Публичный метод деактивации задачи Применимо для публичных задач, когда поставленная цель достигнута или недостижима.

#### Parameters
* `op` The operation

### `public void tcomplete(eosio::name host,uint64_t task_id)` {#classunicore_1a54639a2c971d48407747efdda1b2f2ca}

Публичный метод деактивации задачи Применимо для публичных задач, когда поставленная цель достигнута или недостижима.

#### Parameters
* `op` The operation

### `public void setpgoal(eosio::name host,uint64_t task_id,uint64_t goal_id)` {#classunicore_1aff7586ddb285e938d5228562eb459234}

Метод установки родительской цели

#### Parameters
* `op` The operation

### `public void setdoer(eosio::name host,uint64_t task_id,eosio::name doer)` {#classunicore_1a4ff597c20fe7071fa4d48a1e6fb4e654}

Метод установки исполнителя

#### Parameters
* `op` The operation

### `public void validate(eosio::name host,uint64_t task_id,uint64_t goal_id,eosio::name doer)` {#classunicore_1a5a31fb29df7ae2b8cbf51625b5c2bc21}

Метод ручной валидации задачи

#### Parameters
* `op` The operation

### `public void jointask(eosio::name host,uint64_t task_id,eosio::name doer,std::string comment)` {#classunicore_1a799e41649cd9bab8a1a9a9c41822925b}

Метод предложения себя как исполнителя задачи

#### Parameters
* `op` The operation

### `public void canceljtask(eosio::name host,uint64_t task_id,eosio::name doer)` {#classunicore_1a4d4a35a4c18d428c7e70ff7096cfd47d}

Метод предложения отклонения заявки деятеля

#### Parameters
* `op` The operation

### `public void settaskmeta(eosio::name host,uint64_t task_id,std::string meta)` {#classunicore_1a07802302c610d2dc17f77a3019b81838}

Метод ручной валидации задачи

#### Parameters
* `op` The operation

### `public void setinctask(eosio::name username,uint64_t income_id,bool with_badge,uint64_t my_goal_id,uint64_t my_badge_id)` {#classunicore_1a85136296400d6c3654bc80fcdb835bd3}





### `public void deltask(eosio::name host,uint64_t task_id)` {#classunicore_1a09d7d2d602069a26875aa1eee250d5f4}

Метод удаления задачи Вызывается хостом для удаления поставленной задачи.

#### Parameters
* `op` The operation

### `public void paydebt(eosio::name host,uint64_t goal_id)` {#classunicore_1ab10a203c3d6c37fd1cb71a9110c62e8a}

Метод выплаты долга по цели из числа available в счет объектов долга

#### Parameters
* `op` The new value

### `public void setreport(eosio::name host,eosio::name username,uint64_t task_id,eosio::string data)` {#classunicore_1a178fc39ee1d642454bddeaa3f6084d00}

Публичный метод создания отчета о выполненной задаче Применяется исполнителем задачи для того, чтобы отправить отчет на проверку.

#### Parameters
* `op` The operation

### `public void editreport(eosio::name host,eosio::name username,uint64_t report_id,eosio::string data)` {#classunicore_1ad041a75ade13b77f67f54c99f7ca7d29}

Публиный метод редактирования отчета В случае, если отчет не принят, участник получает возможность отредактировать свой отчет и выслать его на проверку повторно.

#### Parameters
* `op` The operation

### `public void approver(eosio::name host,uint64_t report_id,eosio::string comment)` {#classunicore_1a765b3c6b36dc26922fec8c1236e3d154}

Публичный метод одобрения отчета Используется хостом для того, чтобы принять задачу как выполненную и выдать вознаграждение / награду в виде значка.

#### Parameters
* `op` The operation

### `public void disapprover(eosio::name host,uint64_t report_id,eosio::string comment)` {#classunicore_1a403175c6abbcf360d36ec0259bccc109}

Публичный метод отклонения отчета

#### Parameters
* `op` The operation

### `public void withdrawrepo(eosio::name username,eosio::name host,uint64_t report_id)` {#classunicore_1a9c8a515eef8e3dd262372da54945047b}

Публичный метод вывода баланса отчёта

#### Parameters
* `op` The operation

### `public void distrepo(eosio::name host,uint64_t report_id)` {#classunicore_1ae82cde0f788172b152c6b6896ac75d0d}

Публичный метод обновления баланса отчёта при распределении баланса цели

#### Parameters
* `op` The operation

### `public void vote(eosio::name voter,eosio::name host,uint64_t goal_id,bool up)` {#classunicore_1ac1ca3033e5f65695c938c3d0621e6bfb}





### `public void rvote(eosio::name voter,eosio::name host,uint64_t report_id,bool up)` {#classunicore_1ace8526f2178dd5662a3d96fa1b9b6c7f}





### `public void setcondition(eosio::name host,eosio::string key,uint64_t value)` {#classunicore_1a7b973d2c279fca2fa0cc08c35ebf1abe}





### `public void rmcondition(eosio::name host,uint64_t key)` {#classunicore_1a624a721f564fd28c295bdeef482bacc8}





### `public std::string symbol_to_string(eosio::asset val) const` {#classunicore_1adb0134644edf42ff41b84bca15e6e8ff}

Структура хоста

Returns a string representation of an asset.


#### Parameters
* `val` The value





#### Returns
String representation of the asset.


# struct `ahosts` {#structahosts}


Структура хостов, платформ и их сайтов.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | 
`public eosio::string website` | 
`public eosio::checksum256 hash` | 
`public bool is_host` | 
`public eosio::name type` | 
`public uint64_t votes` | 
`public std::string title` | 
`public std::string purpose` | 
`public std::string manifest` | 
`public bool comments_is_enabled` | 
`public std::string meta` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byvotes() const` | 
`public inline eosio::checksum256 byuuid() const` | 
`public inline eosio::checksum256 hashit(std::string str) const` | 

## Members

### `public eosio::name username` {#structahosts_1a3844566e5baa4b88d0f7eee778fbafbf}





### `public eosio::string website` {#structahosts_1acc9472a06f1e644e3a89a9d53bd8d6d7}





### `public eosio::checksum256 hash` {#structahosts_1a0f6fcb2698373d5ea078e4c3bff4adaa}





### `public bool is_host` {#structahosts_1a02600322cbae5f22e4406b657fad01d3}





### `public eosio::name type` {#structahosts_1a434cfbd0c6623d238104ac340b9aaa5d}





### `public uint64_t votes` {#structahosts_1a0d761a9d91049405b10030079e850e09}





### `public std::string title` {#structahosts_1a9c7a3e787eb79fdf2268b7ee12820f71}





### `public std::string purpose` {#structahosts_1afc6c01b7d25e67f0b88e4326504c4a1f}





### `public std::string manifest` {#structahosts_1a99aeb5eb867801efd8627f2026e29aa6}





### `public bool comments_is_enabled` {#structahosts_1a4a026fb43ada2254c3ba097f7dd25302}





### `public std::string meta` {#structahosts_1a6f085948afc31b2d523e6136df73b3d2}





### `public inline uint64_t primary_key() const` {#structahosts_1a8ff41a09e57821f7cc3387745bc6741a}





### `public inline uint64_t byvotes() const` {#structahosts_1a955bf97cffd800a2dd92b98a880a9542}





### `public inline eosio::checksum256 byuuid() const` {#structahosts_1afe74aae22696c6962ec2d16b355f67b0}





### `public inline eosio::checksum256 hashit(std::string str) const` {#structahosts_1acb8473ca1f7a2e6de6df5afe7fdc710a}






# struct `badges` {#structbadges}


Структура наградных значков хоста Двойной Спирали, их изображений, пиктограмм и предоставляемой силы.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::string caption` | 
`public eosio::string description` | 
`public eosio::string iurl` | 
`public eosio::string pic` | 
`public uint64_t total` | 
`public uint64_t remain` | 
`public uint64_t power` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structbadges_1a6e9efcdda7d0682d696df63d4a87ff75}





### `public eosio::string caption` {#structbadges_1adea9b101a049b24e47cc0f7cca5d2cfa}





### `public eosio::string description` {#structbadges_1a7d0b142c33dbe087f16b5929469b2cdf}





### `public eosio::string iurl` {#structbadges_1aa69f1551abedad7b3c31ac69b8511f85}





### `public eosio::string pic` {#structbadges_1ab7d2eee512677af135bb8ebd494e714a}





### `public uint64_t total` {#structbadges_1a9af45ded7aa632b668f736f8a05ca706}





### `public uint64_t remain` {#structbadges_1a7e4c05e6bb2a93929898b1d0d843b497}





### `public uint64_t power` {#structbadges_1a669d9b961f82256c54236018eaf2e4cf}





### `public inline uint64_t primary_key() const` {#structbadges_1a39faf3dd577d5b5984a905ae56b74a4c}






# struct `balance` {#structbalance}


Структура балансов пользователя у всех хостов Двойной Спирали



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name host` | 
`public eosio::name chost` | 
`public uint64_t cycle_num` | 
`public uint64_t pool_num` | 
`public uint64_t global_pool_id` | 
`public uint64_t quants_for_sale` | 
`public uint64_t next_quants_for_sale` | 
`public uint64_t last_recalculated_win_pool_id` | 
`public bool win` | 
`public int64_t root_percent` | 
`public int64_t convert_percent` | 
`public std::string pool_color` | 
`public eosio::asset available` | 
`public eosio::asset purchase_amount` | 
`public eosio::asset compensator_amount` | 
`public eosio::asset start_convert_amount` | 
`public eosio::asset if_convert` | 
`public eosio::asset if_convert_to_power` | 
`public bool withdrawed` | 
`public std::vector< eosio::asset > forecasts` | 
`public eosio::asset ref_amount` | 
`public eosio::asset dac_amount` | 
`public eosio::asset cfund_amount` | 
`public eosio::asset hfund_amount` | 
`public eosio::asset sys_amount` | 
`public eosio::string meta` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byhost() const` | 

## Members

### `public uint64_t id` {#structbalance_1a3077d63cadc45e37ad1c5dcc5e94ccfd}





### `public eosio::name host` {#structbalance_1aa1b6b2cbd5b9d70037404e5a2ad5dda0}





### `public eosio::name chost` {#structbalance_1a5ec1543098c124fe30f27c214cb8160e}





### `public uint64_t cycle_num` {#structbalance_1ad2ed41f1dfbf60f812a7ac023345930e}





### `public uint64_t pool_num` {#structbalance_1aca9b81261b5367beeaa6fe6e294205ca}





### `public uint64_t global_pool_id` {#structbalance_1a3d37c261621463b5391b79c5db5e1dce}





### `public uint64_t quants_for_sale` {#structbalance_1abbddf497106a7fc59dcce1a0541c657b}





### `public uint64_t next_quants_for_sale` {#structbalance_1aa6c36478cc488c615624e25f53197901}





### `public uint64_t last_recalculated_win_pool_id` {#structbalance_1ad3d998acc801aa7e732e0d4c49fc0040}





### `public bool win` {#structbalance_1aeacfdfcdb555b1bc12109466764fb21f}





### `public int64_t root_percent` {#structbalance_1a43060a5a2d2870df3223afdede2a1902}





### `public int64_t convert_percent` {#structbalance_1ae340e646db87513680649541b7d67127}





### `public std::string pool_color` {#structbalance_1a0044db0c6aa47669ec4a0be274cb5240}





### `public eosio::asset available` {#structbalance_1a2ebfdee4e226a44574200d72cef62b36}





### `public eosio::asset purchase_amount` {#structbalance_1abf8ce9c9100af8a509f46ea113d69b0a}





### `public eosio::asset compensator_amount` {#structbalance_1a3ce58eeeb4209bae31768334fdf7bd42}





### `public eosio::asset start_convert_amount` {#structbalance_1a59fe98e51b77617aefe83d7e41caa7b7}





### `public eosio::asset if_convert` {#structbalance_1a541a27b55aeabff4adcc9140b066205c}





### `public eosio::asset if_convert_to_power` {#structbalance_1a3cf5ede49ed606a3174a69d27367da00}





### `public bool withdrawed` {#structbalance_1abe1f4b170794344c8096d5acbbcd7ef2}





### `public std::vector< eosio::asset > forecasts` {#structbalance_1a05925ffe786d40f4563c8ce1451d55e6}





### `public eosio::asset ref_amount` {#structbalance_1a2a13b04bd7da3263b966014b5cf46eda}





### `public eosio::asset dac_amount` {#structbalance_1ac3e370ea791c5b18846bc914f8cf599a}





### `public eosio::asset cfund_amount` {#structbalance_1aa114410afa216d60a712fb677231ce0b}





### `public eosio::asset hfund_amount` {#structbalance_1ac149b98afd086a41b24b2deb1894b8ea}





### `public eosio::asset sys_amount` {#structbalance_1a050e60d399f399ed44f19793778fd487}





### `public eosio::string meta` {#structbalance_1a70f30d92f79649b752878f9dcc61f7cf}





### `public inline uint64_t primary_key() const` {#structbalance_1a748ad87c69d0195c26a690857b427cc9}





### `public inline uint64_t byhost() const` {#structbalance_1a728c8bb1d05ff085aca6d4f3d4ee3e02}






# struct `benefactors` {#structbenefactors}


Структура бенефакторов цели хоста Двойной Спирали



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t goal_id` | 
`public eosio::name benefactor` | 
`public uint64_t weight` | 
`public eosio::asset income` | 
`public uint128_t income_in_segments` | 
`public eosio::asset withdrawed` | 
`public eosio::asset income_limit` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bygoalid() const` | 
`public inline uint128_t bybengoal() const` | 

## Members

### `public uint64_t id` {#structbenefactors_1a677d15f6844aa2618566bca02c20808f}





### `public uint64_t goal_id` {#structbenefactors_1a7b9d60f025b5db37474b888ad430cee6}





### `public eosio::name benefactor` {#structbenefactors_1ae2840b5c590b2da9ed8d0c8e91ad2269}





### `public uint64_t weight` {#structbenefactors_1a0c58861df063615af17d5aca5ac4761c}





### `public eosio::asset income` {#structbenefactors_1aae6f5d97fb66739c2b6a41914be408ec}





### `public uint128_t income_in_segments` {#structbenefactors_1a850b07383b638938edde052640f30512}





### `public eosio::asset withdrawed` {#structbenefactors_1a725375e8dab014236bbbcd9e9366b80d}





### `public eosio::asset income_limit` {#structbenefactors_1a355c9adaf5e7f448493a256a1f8b4edd}





### `public inline uint64_t primary_key() const` {#structbenefactors_1ad365cc6b12b704cfc599811ce62e2af6}





### `public inline uint64_t bygoalid() const` {#structbenefactors_1aea4edf7d9708f8b4ad53249b417ee5fc}





### `public inline uint128_t bybengoal() const` {#structbenefactors_1a9fdd8b8be320de55a7f08424b0e94a6f}






# struct `bwtradegraph` {#structbwtradegraph}


Структура для отображения Двойной Спирали в виде торгового графика типа BLACK-AND-WHITE.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t pool_id` | 
`public uint64_t cycle_num` | 
`public uint64_t pool_num` | 
`public uint64_t open` | 
`public uint64_t high` | 
`public uint64_t low` | 
`public uint64_t close` | 
`public bool is_white` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bycycle() const` | 

## Members

### `public uint64_t pool_id` {#structbwtradegraph_1acfa9ef5e4f6d67b1957f0d48b432c824}





### `public uint64_t cycle_num` {#structbwtradegraph_1a1d72d90e14e12cfcf6f013b4a1616ef5}





### `public uint64_t pool_num` {#structbwtradegraph_1aa1fe7048fd9317355d7e1a29cc8a46e5}





### `public uint64_t open` {#structbwtradegraph_1ab7b42c19235f0979ed66f2582cd63fa8}





### `public uint64_t high` {#structbwtradegraph_1af013eda31c295b475060840c0898d4f6}





### `public uint64_t low` {#structbwtradegraph_1a303e09c6464e5eebde73ee5222fb371a}





### `public uint64_t close` {#structbwtradegraph_1a6c83afcc1c4c2e5882a13c7c2da02c54}





### `public bool is_white` {#structbwtradegraph_1a138cd8201cbb657b3dcbeba547c9c279}





### `public inline uint64_t primary_key() const` {#structbwtradegraph_1a19590e9b3e4adbd376cdf46c1eacc87c}





### `public inline uint64_t bycycle() const` {#structbwtradegraph_1a978bf2b0e04ef88c0370b3989fe85e73}






# struct `cmsconfig` {#structcmsconfig}


Структура конфигурации платформы.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::string config` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structcmsconfig_1a242e7cb1cc8bbfe1b25f16b8b45597bb}





### `public eosio::string config` {#structcmsconfig_1a79dffc55a16afae47f1aa6746dfa97d5}





### `public inline uint64_t primary_key() const` {#structcmsconfig_1a5cc5281bf9358c20832663492af8e9f9}






# struct `cmscontent` {#structcmscontent}


Структура хранилища контента и оформления платформ.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name type` | 
`public eosio::name lang` | 
`public eosio::string content` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bylang() const` | 

## Members

### `public eosio::name type` {#structcmscontent_1a728f21709bd37994b31c334cc2d91888}





### `public eosio::name lang` {#structcmscontent_1a954b44d1b345443ea24776a2cd46179d}





### `public eosio::string content` {#structcmscontent_1aedaaa678ac48fc729cf9de6da8a9b927}





### `public inline uint64_t primary_key() const` {#structcmscontent_1ac36e99c6076f64079d59ac390593a3c9}





### `public inline uint64_t bylang() const` {#structcmscontent_1a97ea071f0bbbdfb0dcb997761e22b5ce}






# struct `conditions` {#structconditions}


Структура хранилища универсального набора условий, относящихся к хосту, платформе или протоколу.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t key` | 
`public eosio::string key_string` | 
`public uint64_t value` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t key` {#structconditions_1a50f42a1945a457079ae63b6e18bfca4a}





### `public eosio::string key_string` {#structconditions_1a1b699fa747d89b818d097018b6fb4d89}





### `public uint64_t value` {#structconditions_1af109613af52693bfd8c089c203e12a9f}





### `public inline uint64_t primary_key() const` {#structconditions_1a234a5327f24d23fc88b500cd2309b57a}






# struct `market` {#structmarket}


Структура взамодействия с рынками торгового робота Bancor.

Uses Bancor math to create a 50/50 relay between two asset types. The state of the bancor exchange is entirely contained within this struct. There are no external side effects associated with using this API.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``connector``](#structmarket_1_1connector)        | Структура коннектора рынка торгового робота Bancor.
`public uint64_t id` | 
`public std::string name` | 
`public uint64_t vesting_seconds` | 
`public eosio::asset supply` | 
`public `[`connector`](#structmarket_1_1connector)` base` | 
`public `[`connector`](#structmarket_1_1connector)` quote` | 
`public inline uint64_t primary_key() const` | 
`public asset convert_to_exchange(`[`connector`](#structmarket_1_1connector)` & reserve,const asset & payment)` | 
`public asset convert_from_exchange(`[`connector`](#structmarket_1_1connector)` & reserve,const asset & tokens)` | 
`public asset convert(const asset & from,const symbol & to)` | 
`public asset direct_convert(const asset & from,const symbol & to)` | 

## Members

### `struct `[``connector``](#structmarket_1_1connector) {#structmarket_1_1connector}

Структура коннектора рынка торгового робота Bancor.


### `public uint64_t id` {#structmarket_1ae532d3fe0cf32f7ce146ac8dc471193d}





### `public std::string name` {#structmarket_1a193ab300909085d98571159a2ac0f525}





### `public uint64_t vesting_seconds` {#structmarket_1a14276e13986dd8f9467e4f46cbe36f74}





### `public eosio::asset supply` {#structmarket_1a43fd06e1fccaf87a936e0cbf98e4d6a6}





### `public `[`connector`](#structmarket_1_1connector)` base` {#structmarket_1a2c729f8f225ad991298224c1b5b8afee}





### `public `[`connector`](#structmarket_1_1connector)` quote` {#structmarket_1a7328f8895180f466a1354c2e3110a54b}





### `public inline uint64_t primary_key() const` {#structmarket_1a927be547fb7499a82fc5a717e133a7b4}





### `public asset convert_to_exchange(`[`connector`](#structmarket_1_1connector)` & reserve,const asset & payment)` {#structmarket_1a0f2926879c20545177fe18c177420bdb}





### `public asset convert_from_exchange(`[`connector`](#structmarket_1_1connector)` & reserve,const asset & tokens)` {#structmarket_1a868ca81f6e115b7c6d3f3894cc6bc971}





### `public asset convert(const asset & from,const symbol & to)` {#structmarket_1ab329c1ef0f3b4569b0d1786736cc3794}





### `public asset direct_convert(const asset & from,const symbol & to)` {#structmarket_1a5d5041e0aaf2cb29bad70644d264d579}






# struct `connector` {#structmarket_1_1connector}


Структура коннектора рынка торгового робота Bancor.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::asset balance` | 
`public double weight` | 
`public eosio::name contract` | 

## Members

### `public eosio::asset balance` {#structmarket_1_1connector_1ad219384bf98be14060d3ffc95368a0d9}





### `public double weight` {#structmarket_1_1connector_1a8c91ee071e79b355dbdcaa47e57c94e4}





### `public eosio::name contract` {#structmarket_1_1connector_1a07c202c4ae0204d5b71ddccdf4dc9169}






# struct `exchange_state` {#structexchange__state}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``connector``](#structexchange__state_1_1connector)        | 
`public asset supply` | 
`public `[`connector`](#structexchange__state_1_1connector)` base` | 
`public `[`connector`](#structexchange__state_1_1connector)` quote` | 
`public inline uint64_t primary_key() const` | 
`public asset convert_to_exchange(`[`connector`](#structexchange__state_1_1connector)` & c,asset in)` | 
`public asset convert_from_exchange(`[`connector`](#structexchange__state_1_1connector)` & c,asset in)` | 
`public asset convert(asset from,const symbol & to)` | 

## Members

### `struct `[``connector``](#structexchange__state_1_1connector) {#structexchange__state_1_1connector}




### `public asset supply` {#structexchange__state_1aed724d12bf57c748a1bd1cae697323e3}





### `public `[`connector`](#structexchange__state_1_1connector)` base` {#structexchange__state_1ac347aedb89368530adf4740be02822bd}





### `public `[`connector`](#structexchange__state_1_1connector)` quote` {#structexchange__state_1a72f37d7232ee04314ea2096e923c6ad5}





### `public inline uint64_t primary_key() const` {#structexchange__state_1a7891948a4c98433065f6297a846770b4}





### `public asset convert_to_exchange(`[`connector`](#structexchange__state_1_1connector)` & c,asset in)` {#structexchange__state_1ac3dab3448c9d5bb55770d33d95f688ba}





### `public asset convert_from_exchange(`[`connector`](#structexchange__state_1_1connector)` & c,asset in)` {#structexchange__state_1a65e34e352f066f1dae0385b2e6aa333e}





### `public asset convert(asset from,const symbol & to)` {#structexchange__state_1aa791a6a6eac2f7a8c7b5615bc57f177b}






# struct `connector` {#structexchange__state_1_1connector}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public asset balance` | 
`public double weight` | 

## Members

### `public asset balance` {#structexchange__state_1_1connector_1acff5aa7b2a08972035cf573a67c1253b}





### `public double weight` {#structexchange__state_1_1connector_1aaefdf596a43f0b04a3193a9831a399b1}






# struct `coredhistory` {#structcoredhistory}


Структура для хранения сообщений в режиме чата ограниченной длинны от спонсоров хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t pool_id` | 
`public eosio::name username` | 
`public std::string action` | 
`public std::string message` | 
`public eosio::asset amount` | 
`public inline uint64_t primary_key() const` | 
`public  EOSLIB_SERIALIZE(`[`coredhistory`](#structcoredhistory)`,(id)(pool_id)(username)(action)(message)(amount))` | 

## Members

### `public uint64_t id` {#structcoredhistory_1a1ea9f7d117c11540b58739b550f87091}





### `public uint64_t pool_id` {#structcoredhistory_1a94bdb8112e0158dd245686615a984781}





### `public eosio::name username` {#structcoredhistory_1a8bf1a39cea053b8bde6e5aa59c811477}





### `public std::string action` {#structcoredhistory_1a32396e9e3e997367f5ec62d5b008733d}





### `public std::string message` {#structcoredhistory_1a237f39d70f67537ea87e0c773ef0e936}





### `public eosio::asset amount` {#structcoredhistory_1a0916b76d450651176c2a245879af398a}





### `public inline uint64_t primary_key() const` {#structcoredhistory_1a24de1f8333ffcd0a24742c03bb2218ba}





### `public  EOSLIB_SERIALIZE(`[`coredhistory`](#structcoredhistory)`,(id)(pool_id)(username)(action)(message)(amount))` {#structcoredhistory_1a3238d8500fdc23f1d16513dae4afc7eb}






# struct `cpartners2` {#structcpartners2}


Структура учёта партнёров и их статусов у хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name partner` | 
`public eosio::name status` | 
`public eosio::time_point_sec join_at` | 
`public eosio::time_point_sec expiration` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name partner` {#structcpartners2_1aaf8d230c6041cc9558ab5a8aaf5fcc19}





### `public eosio::name status` {#structcpartners2_1a6d964e260d55c19e8083af67aa853562}





### `public eosio::time_point_sec join_at` {#structcpartners2_1a7d5331f9cc298535c4114faa7cdb90d5}





### `public eosio::time_point_sec expiration` {#structcpartners2_1a96eb13b5f403f2268a7abc3bfa4f81d3}





### `public inline uint64_t primary_key() const` {#structcpartners2_1a492e088c954ac116e444dea32687f537}






# struct `currency_stats` {#structcurrency__stats}


Структура статистики жетонов в обороте



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::asset supply` | 
`public eosio::asset max_supply` | 
`public eosio::name issuer` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::asset supply` {#structcurrency__stats_1a1afb7ac2a502c69231cc27aebd720a45}





### `public eosio::asset max_supply` {#structcurrency__stats_1a9da25dbd496e340784f978aa353b8679}





### `public eosio::name issuer` {#structcurrency__stats_1a9e27c17d1a33bc5a16cb502c96b1695b}





### `public inline uint64_t primary_key() const` {#structcurrency__stats_1a54eba2ce368a5195026fd89ca505d28b}






# struct `cycle` {#structcycle}


Структура для учёта развития циклов хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name ahost` | 
`public uint64_t start_at_global_pool_id` | 
`public uint64_t finish_at_global_pool_id` | 
`public eosio::asset emitted` | 
`public inline uint64_t primary_key() const` | 
`public  EOSLIB_SERIALIZE(`[`cycle`](#structcycle)`,(id)(ahost)(start_at_global_pool_id)(finish_at_global_pool_id)(emitted))` | 

## Members

### `public uint64_t id` {#structcycle_1a2dae7ac3b8113c0b0366126e042e0205}





### `public eosio::name ahost` {#structcycle_1abf11d98232387603ab12ae556257c814}





### `public uint64_t start_at_global_pool_id` {#structcycle_1acabb7980124283136d9df215a53e8397}





### `public uint64_t finish_at_global_pool_id` {#structcycle_1aa414b190d5404ada4c3c98e624d5d934}





### `public eosio::asset emitted` {#structcycle_1afb69f93b40d1614cfe006ca17212f801}





### `public inline uint64_t primary_key() const` {#structcycle_1af8cc307aa90e11b78548dd6921ac9337}





### `public  EOSLIB_SERIALIZE(`[`cycle`](#structcycle)`,(id)(ahost)(start_at_global_pool_id)(finish_at_global_pool_id)(emitted))` {#structcycle_1a584fb1cb198a96fb12b9bc0cd5447f6c}






# struct `cycle3` {#structcycle3}


Структура для учёта сжигания внутренней конвертационной единицы.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t burned_quants` | 
`public uint64_t black_burned_quants` | 
`public uint64_t white_burned_quants` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structcycle3_1a3cf542d190bd8cd0fbf14940da5842e4}





### `public uint64_t burned_quants` {#structcycle3_1a392a76c646ae64580bbfec515c7d9448}





### `public uint64_t black_burned_quants` {#structcycle3_1a6a62a7509628dbddfebe013acdf253b8}





### `public uint64_t white_burned_quants` {#structcycle3_1a06fdfa87b0e7088a600ac84eb55c3171}





### `public inline uint64_t primary_key() const` {#structcycle3_1a004564991f58638fb60be5a624f69496}






# struct `dacs` {#structdacs}


Структура команды хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name dac` | 
`public eosio::name limit_type` | 
`public uint64_t weight` | 
`public eosio::asset income` | 
`public uint128_t income_in_segments` | 
`public eosio::asset withdrawed` | 
`public eosio::asset income_limit` | 
`public eosio::time_point_sec last_pay_at` | 
`public eosio::time_point_sec created_at` | 
`public std::string role` | 
`public std::string description` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bylimittype() const` | 

## Members

### `public eosio::name dac` {#structdacs_1ae9be077b76903051cf3e9fbafe071bda}





### `public eosio::name limit_type` {#structdacs_1a88cfdd3de941fb47f104af9ebc62e51c}





### `public uint64_t weight` {#structdacs_1abd0b1806d25461cd48b36370bb6e7c09}





### `public eosio::asset income` {#structdacs_1a8127a0b285195817c1ddfed691d202cb}





### `public uint128_t income_in_segments` {#structdacs_1a814ea7767e5b315cfc1f3328208e3aba}





### `public eosio::asset withdrawed` {#structdacs_1ad43317ebc8a377915fe01394f74164b8}





### `public eosio::asset income_limit` {#structdacs_1ac0ceba57e94b8f3e70e504d85f1f41cd}





### `public eosio::time_point_sec last_pay_at` {#structdacs_1abc7762bb445e20f54dc0d447cfc67bce}





### `public eosio::time_point_sec created_at` {#structdacs_1a64d9fdad557feddad6be7f4feb2954df}





### `public std::string role` {#structdacs_1a797a7e46d89522ddb66581320034eb82}





### `public std::string description` {#structdacs_1afb89d957d0f8bda654b149bf04627ddf}





### `public inline uint64_t primary_key() const` {#structdacs_1adbff85b09633fb360200e8571993d43f}





### `public inline uint64_t bylimittype() const` {#structdacs_1a3feeefb64e7c89474ba70e76277ebd28}






# struct `debts` {#structdebts}


Структура целевого долга хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t goal_id` | 
`public eosio::name username` | 
`public eosio::asset amount` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bygoal() const` | 
`public inline uint64_t byusername() const` | 

## Members

### `public uint64_t id` {#structdebts_1a6b66af46bb77f8d69f348666b7101c76}





### `public uint64_t goal_id` {#structdebts_1a0c81af97bc2a68cc5b9f69cc1764cae9}





### `public eosio::name username` {#structdebts_1aca867fc824e45f7e0bb7917f22166cc2}





### `public eosio::asset amount` {#structdebts_1a22ed3e73b254ae3781eee307713c4d23}





### `public inline uint64_t primary_key() const` {#structdebts_1adc67177f964ad4b62951d18b6fb02344}





### `public inline uint64_t bygoal() const` {#structdebts_1ad47725f71d772fe4a492b2dcb1b9eec7}





### `public inline uint64_t byusername() const` {#structdebts_1a55268520b4adab0ca4058af1da834fb6}






# struct `delegations` {#structdelegations}


Структура учёта делегированной силы от пользователя к пользователю.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name reciever` | 
`public uint64_t shares` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name reciever` {#structdelegations_1a9a1bdcef11d0a6e16a408e2580cf2df7}





### `public uint64_t shares` {#structdelegations_1a7787306eec01bd3dace649a99a46adcd}





### `public inline uint64_t primary_key() const` {#structdelegations_1ac9b8c9b5d0dda7e6f9008cf1ba64b697}






# struct `dlog` {#structdlog}


Структура истории получения безусловного потока жетонов пользователем у хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name host` | 
`public uint64_t pool_id` | 
`public uint128_t segments` | 
`public eosio::asset amount` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structdlog_1a631b025f45d95a13490dd5c308f3a9d9}





### `public eosio::name host` {#structdlog_1a2773a44a4c1f8ea4fbf862afa7e43b25}





### `public uint64_t pool_id` {#structdlog_1af12e25c20f606373a8355a30bbf78c79}





### `public uint128_t segments` {#structdlog_1a013dba87b3a6b4aebd044a54b1e63607}





### `public eosio::asset amount` {#structdlog_1a83cdd3a33e2cdea1831f1a406fbd4e6f}





### `public inline uint64_t primary_key() const` {#structdlog_1a475232f214d913d1ce04dc0d78f90a35}






# struct `doers` {#structdoers}


Структура деятелей по задаче.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t task_id` | 
`public eosio::name doer` | 
`public bool is_host` | 
`public uint64_t doer_goal_id` | 
`public uint64_t doer_badge_id` | 
`public std::string comment` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byusername() const` | 
`public inline uint128_t doerwithtask() const` | 

## Members

### `public uint64_t id` {#structdoers_1ab8e912e3cf6661c09ff633f665398a72}





### `public uint64_t task_id` {#structdoers_1a6b3c86f2da155dd9d50d006b9350a442}





### `public eosio::name doer` {#structdoers_1ac5f40777c970cd689e869935dff9e315}





### `public bool is_host` {#structdoers_1a8929f96a2f79a78e4a411fe388ac16d2}





### `public uint64_t doer_goal_id` {#structdoers_1abea11e633506933a23e48197658e32a2}





### `public uint64_t doer_badge_id` {#structdoers_1a6077b10bd669ac91026736023492a115}





### `public std::string comment` {#structdoers_1aa29368854c3b0633b6f88367eb11d005}





### `public inline uint64_t primary_key() const` {#structdoers_1a3e2bed451801cf7ce95fd740aec789bb}





### `public inline uint64_t byusername() const` {#structdoers_1ad22419489100a8a16e7e236f601bb9d6}





### `public inline uint128_t doerwithtask() const` {#structdoers_1afc148c5ba07006792a44fc4166050c9e}






# struct `emission` {#structemission}


Структура параметров эмиссии целевого фонда хоста Двойной Спирали



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name host` | 
`public uint64_t percent` | 
`public uint64_t gtop` | 
`public eosio::asset fund` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name host` {#structemission_1a50f41aa67f2ebcedbaad90618ece068b}





### `public uint64_t percent` {#structemission_1a90a071303b94d463f402fea47b624a3e}





### `public uint64_t gtop` {#structemission_1a4000c3453e37c4414832ad05111b05f1}





### `public eosio::asset fund` {#structemission_1a4b716d4513ce0c923116c020cede0384}





### `public inline uint64_t primary_key() const` {#structemission_1a3b8f921b625966c1af4f8f346247f58a}






# struct `funds` {#structfunds}


Структура глобальных фондов владельцев жетонов, помещенных на распределение.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name issuer` | 
`public eosio::name token_contract` | 
`public eosio::asset fund` | 
`public std::string descriptor` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byissuer() const` | 
`public inline uint128_t codeandsmbl() const` | 

## Members

### `public uint64_t id` {#structfunds_1a9739e08a3d3556a7f73552bcb370b213}





### `public eosio::name issuer` {#structfunds_1a539c18e111c5d748cec58b15e10ea0a8}





### `public eosio::name token_contract` {#structfunds_1a3ca4e48b1ef43580b0bb1d52d4ee1ca3}





### `public eosio::asset fund` {#structfunds_1ae1327e44f67595f7bc0d30ee9d436d29}





### `public std::string descriptor` {#structfunds_1aeb379a7b45ab82e8961582b9e5fc6eae}





### `public inline uint64_t primary_key() const` {#structfunds_1afdc3eb8e502cb03e6dacd48605ccc671}





### `public inline uint64_t byissuer() const` {#structfunds_1a8b5c509d5fe2b1aef52afba800f29929}





### `public inline uint128_t codeandsmbl() const` {#structfunds_1abdba3d22c4cde341d9bd7638462e65bd}






# struct `goals` {#structgoals}


Структура целей хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t parent_id` | 
`public eosio::name type` | 
`public eosio::name creator` | 
`public eosio::name benefactor` | 
`public eosio::name host` | 
`public eosio::name status` | 
`public bool is_batch` | 
`public std::vector< uint64_t > batch` | 
`public uint64_t benefactors_weight` | 
`public eosio::time_point_sec created` | 
`public eosio::time_point_sec start_at` | 
`public eosio::time_point_sec finish_at` | 
`public eosio::time_point_sec expired_at` | 
`public uint64_t duration` | 
`public uint64_t priority` | 
`public uint64_t cashback` | 
`public uint64_t participants_count` | 
`public std::string parent_permlink` | 
`public std::string permlink` | 
`public std::string title` | 
`public std::string description` | 
`public eosio::asset target` | 
`public uint64_t debt_count` | 
`public eosio::asset target1` | 
`public eosio::asset target2` | 
`public eosio::asset target3` | 
`public eosio::asset available` | 
`public eosio::asset debt_amount` | 
`public int64_t total_votes` | 
`public uint64_t total_tasks` | 
`public bool validated` | 
`public bool activated` | 
`public bool filled` | 
`public bool reported` | 
`public bool checked` | 
`public bool comments_is_enabled` | 
`public eosio::name who_can_create_tasks` | 
`public std::string report` | 
`public eosio::asset withdrawed` | 
`public std::vector< eosio::name > voters` | 
`public std::string meta` | 
`public uint64_t gifted_badges` | 
`public uint64_t gifted_power` | 
`public uint64_t reports_count` | 
`public bool is_encrypted` | 
`public std::string public_key` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byvotes() const` | 
`public inline uint64_t bytype() const` | 
`public inline uint64_t bystatus() const` | 
`public inline uint64_t byparentid() const` | 
`public inline uint64_t byfilled() const` | 
`public inline uint64_t bycreated() const` | 
`public inline uint64_t bypriority() const` | 
`public inline uint64_t byusername() const` | 
`public inline uint64_t byhost() const` | 
`public inline uint128_t by_username_and_host() const` | 

## Members

### `public uint64_t id` {#structgoals_1a8eba3dc0dab674064061651c6eb71779}





### `public uint64_t parent_id` {#structgoals_1a2140de541b014cae48b3ac810f4ebb05}





### `public eosio::name type` {#structgoals_1a041e781eb67475545e0bdf8f52df94dc}





### `public eosio::name creator` {#structgoals_1a29e945bda7a528dc7bd80f90395708c8}





### `public eosio::name benefactor` {#structgoals_1a7dc6a3016c29afd4c500f176063b5d05}





### `public eosio::name host` {#structgoals_1a94172280727369516f5570c3dca488a6}





### `public eosio::name status` {#structgoals_1ab3105f1413e48499857aaf65829566a6}





### `public bool is_batch` {#structgoals_1a354e45b01bf2b9d919346aca6d275741}





### `public std::vector< uint64_t > batch` {#structgoals_1afb84640a894d3e6757392439396d9dc0}





### `public uint64_t benefactors_weight` {#structgoals_1a328ac2d2c21b5b926d97b315ab274faa}





### `public eosio::time_point_sec created` {#structgoals_1a0ecedf561b672247e629495d0d766b9e}





### `public eosio::time_point_sec start_at` {#structgoals_1a5f1d91c6770c6a5e6c8a1d299dabae6b}





### `public eosio::time_point_sec finish_at` {#structgoals_1abec8eece24c1cd0febe0d20448b0e3bd}





### `public eosio::time_point_sec expired_at` {#structgoals_1a054538540aa86753bde7bce3094c0a6b}





### `public uint64_t duration` {#structgoals_1a2e0dc500739ead42a7cfdbfd40145974}





### `public uint64_t priority` {#structgoals_1a08c4cae575347e4a488c2faa683675e8}





### `public uint64_t cashback` {#structgoals_1af480984561a63a25749e9966d4bfc37d}





### `public uint64_t participants_count` {#structgoals_1a22853976bd6c35645ee53f4e0cf57748}





### `public std::string parent_permlink` {#structgoals_1a08fdb21801bdc67fd56c3aa7b7039309}





### `public std::string permlink` {#structgoals_1a4eeb86cdbe42120c4945d566fb24889e}





### `public std::string title` {#structgoals_1a11180e768d20241137069484b9c9cb65}





### `public std::string description` {#structgoals_1a8a42593c3da900821bbd980f85c8fb51}





### `public eosio::asset target` {#structgoals_1a0a521876420d2c69fe308854d5c48fa1}





### `public uint64_t debt_count` {#structgoals_1af44690442247cc7759c9c5e14b9b8146}





### `public eosio::asset target1` {#structgoals_1aa60c4415980afa5747628a8487bf57db}





### `public eosio::asset target2` {#structgoals_1a0d8f605a8ef5b750312a4ef25447e8c5}





### `public eosio::asset target3` {#structgoals_1a0c7e1384356a6092cc92dfb5c86e23cb}





### `public eosio::asset available` {#structgoals_1a833e3922b9ed3788cdac61c3762d15ce}





### `public eosio::asset debt_amount` {#structgoals_1a50c7f7557639a42246ae4a75c986b881}





### `public int64_t total_votes` {#structgoals_1a3b02eb227e5e2bd30439fcdb425825c9}





### `public uint64_t total_tasks` {#structgoals_1a5557be4e2582c641a4d8971eed91dd81}





### `public bool validated` {#structgoals_1ac3efbc316b87f181069e6eaae900a29b}





### `public bool activated` {#structgoals_1afd5c299ccd909ab3acc36d257d019a0b}





### `public bool filled` {#structgoals_1a7180729ad741602e1be314c90b833782}





### `public bool reported` {#structgoals_1a027698ac11a5d8e177ed41a7a22c5a79}





### `public bool checked` {#structgoals_1a66e73d88208e0db0cefc7eae005ceb77}





### `public bool comments_is_enabled` {#structgoals_1af0cdb2a4a5b26b5cc5b4b703979f0b15}





### `public eosio::name who_can_create_tasks` {#structgoals_1a26f12e3cf4bd9b71bcc23835b0218f13}





### `public std::string report` {#structgoals_1af4397668a230464d201d04d2d681a396}





### `public eosio::asset withdrawed` {#structgoals_1a7be33b74b9775c4a81eccc70c88287af}





### `public std::vector< eosio::name > voters` {#structgoals_1abb90a7b4a33e2e4f1998ed1b67ecd3c2}





### `public std::string meta` {#structgoals_1a556bed0c2aa8e8199bba15f7e547bb2b}





### `public uint64_t gifted_badges` {#structgoals_1a7bce6066ed77bb9d9b2276b7a4a6608f}





### `public uint64_t gifted_power` {#structgoals_1a8354980258d5a0080bac28d7823c71c1}





### `public uint64_t reports_count` {#structgoals_1a8cbd1349aadc5c372444b234d55bed57}





### `public bool is_encrypted` {#structgoals_1a50495b51590f77692594d4e9cffc8ab9}





### `public std::string public_key` {#structgoals_1a3fb010568bcb02eb1f3bee688471fb82}





### `public inline uint64_t primary_key() const` {#structgoals_1a0f596c490585385d6c3b97bb7f836e88}





### `public inline uint64_t byvotes() const` {#structgoals_1a35bab0d724d1a6c708132c66523a4c4b}





### `public inline uint64_t bytype() const` {#structgoals_1ad49889434ae2b7d6b90a128de3dc3a9f}





### `public inline uint64_t bystatus() const` {#structgoals_1a7d12f5bc6b9ec3585aedc6f88027cc25}





### `public inline uint64_t byparentid() const` {#structgoals_1a9a29a715bd4420ff1ac6fdbb2c87fc1b}





### `public inline uint64_t byfilled() const` {#structgoals_1af7c5070af0c06aec15bd030c4a3c9ea9}





### `public inline uint64_t bycreated() const` {#structgoals_1a47f474aedab8dfa47c728067b49e07d1}





### `public inline uint64_t bypriority() const` {#structgoals_1a81da6a1176d4c433a7948db72954a0fa}





### `public inline uint64_t byusername() const` {#structgoals_1ae9639cc0f759eb74705f68947f773107}





### `public inline uint64_t byhost() const` {#structgoals_1a523680b1ce247d7a24187f0fcac9962d}





### `public inline uint128_t by_username_and_host() const` {#structgoals_1ac35b433e7b0d7bed8f2041a093b4d0d4}






# struct `goals3` {#structgoals3}


Структура целей хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::asset total_on_distribution` | 
`public eosio::asset remain_on_distribution` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structgoals3_1a9eea3f010911379e6f4ead29d5a3fe71}





### `public eosio::asset total_on_distribution` {#structgoals3_1a008e54ebad1f5d93e022501b3b3fc012}





### `public eosio::asset remain_on_distribution` {#structgoals3_1a8733ab368d84a3588b197e1b58a08c32}





### `public inline uint64_t primary_key() const` {#structgoals3_1a2d06e31d7cef3d14996bb5f49b04fd66}






# struct `goalspartic` {#structgoalspartic}


Структура участников цели хоста Двойной Спирали



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t goal_id` | 
`public eosio::name username` | 
`public eosio::name role` | 
`public bool completed` | 
`public eosio::time_point_sec expiration` | 
`public inline uint64_t primary_key() const` | 
`public inline uint128_t byusergoal() const` | 
`public inline uint64_t byusername() const` | 
`public inline uint64_t byrole() const` | 

## Members

### `public uint64_t id` {#structgoalspartic_1a4a827ab1581493d26766fc29ed68933e}





### `public uint64_t goal_id` {#structgoalspartic_1a620bde3995640d4acc18208df2f8264d}





### `public eosio::name username` {#structgoalspartic_1a5ae546f7c77d86d1957a92477b7a5b55}





### `public eosio::name role` {#structgoalspartic_1aed6900ac72c882538987c931b8f035b4}





### `public bool completed` {#structgoalspartic_1ae9528d5bc4ed1f26b120954d5f4b3569}





### `public eosio::time_point_sec expiration` {#structgoalspartic_1a7ddf89793918e5a656ce62408ed44808}





### `public inline uint64_t primary_key() const` {#structgoalspartic_1aafafd092a2217ce1b8149850841d1ba8}





### `public inline uint128_t byusergoal() const` {#structgoalspartic_1a1b4b454d41971172fac09aa441f44767}





### `public inline uint64_t byusername() const` {#structgoalspartic_1a642ff9cef3c400e4e65024c5eb6daabb}





### `public inline uint64_t byrole() const` {#structgoalspartic_1a7b9500ef1b7b296b0d070125dcff7606}






# struct `gpercents` {#structgpercents}


Структура системного процента, изымаего протокол из обращения при каждом полу-обороте Двойной Спирали каждого хоста.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name key` | 
`public uint64_t value` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name key` {#structgpercents_1a3590265e98954a83b5dcee6622a12a2d}





### `public uint64_t value` {#structgpercents_1ab02cef50ba01b31a15e8041e80b57c8e}





### `public inline uint64_t primary_key() const` {#structgpercents_1a68f903552db4ec01f5a5b72e66b3242c}






# struct `guests` {#structguests}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | 
`public eosio::name registrator` | 
`public eosio::public_key public_key` | 
`public eosio::asset cpu` | 
`public eosio::asset net` | 
`public bool set_referer` | 
`public eosio::time_point_sec expiration` | 
`public eosio::asset to_pay` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byexpr() const` | 

## Members

### `public eosio::name username` {#structguests_1a3c550ec3e80d040f7d95f7c9ee500dbe}





### `public eosio::name registrator` {#structguests_1a22effafdbf30a59852f7f7cd82e7b531}





### `public eosio::public_key public_key` {#structguests_1ad2754df2d77b869bd0c4e7e9cedad6a7}





### `public eosio::asset cpu` {#structguests_1ad1522a74eca90485897b549c857fa4b4}





### `public eosio::asset net` {#structguests_1a36f81702c9834a45c9ecae49588b2e05}





### `public bool set_referer` {#structguests_1a201b3b738384f53f13965466ef3974d8}





### `public eosio::time_point_sec expiration` {#structguests_1a33cb839d82a59aadd3dbc7879f8b56cf}





### `public eosio::asset to_pay` {#structguests_1a4ac3236a747b7c920f2675a9f8fe35da}





### `public inline uint64_t primary_key() const` {#structguests_1aa44a7691d4254e2f580a64791b061d70}





### `public inline uint64_t byexpr() const` {#structguests_1a1f409e9372ba160ae769a2057decf311}






# struct `hosts` {#structhosts}


Структура хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | 
`public eosio::time_point_sec registered_at` | 
`public eosio::name architect` | 
`public eosio::name hoperator` | 
`public eosio::name type` | 
`public eosio::name chat_mode` | 
`public uint64_t consensus_percent` | 
`public uint64_t referral_percent` | 
`public uint64_t dacs_percent` | 
`public uint64_t cfund_percent` | 
`public uint64_t hfund_percent` | 
`public uint64_t sys_percent` | 
`public std::vector< uint64_t > levels` | 
`public std::vector< eosio::name > gsponsor_model` | 
`public bool direct_goal_withdraw` | 
`public uint64_t dac_mode` | 
`public uint64_t total_dacs_weight` | 
`public eosio::name ahost` | 
`public std::vector< eosio::name > chosts` | 
`public bool sale_is_enabled` | 
`public eosio::name sale_mode` | 
`public eosio::name sale_token_contract` | 
`public eosio::asset asset_on_sale` | 
`public uint64_t asset_on_sale_precision` | 
`public std::string asset_on_sale_symbol` | 
`public int64_t sale_shift` | 
`public bool non_active_chost` | 
`public bool need_switch` | 
`public uint64_t fhosts_mode` | 
`public std::vector< eosio::name > fhosts` | 
`public std::string title` | 
`public std::string purpose` | 
`public bool voting_only_up` | 
`public eosio::name power_market_id` | 
`public uint64_t total_shares` | 
`public eosio::asset quote_amount` | 
`public eosio::name quote_token_contract` | 
`public std::string quote_symbol` | 
`public uint64_t quote_precision` | 
`public eosio::name root_token_contract` | 
`public eosio::asset root_token` | 
`public std::string symbol` | 
`public uint64_t precision` | 
`public eosio::asset to_pay` | 
`public bool payed` | 
`public uint64_t cycle_start_id` | 
`public uint64_t current_pool_id` | 
`public uint64_t current_cycle_num` | 
`public uint64_t current_pool_num` | 
`public bool parameters_setted` | 
`public bool activated` | 
`public bool priority_flag` | 
`public uint64_t total_goals` | 
`public uint64_t achieved_goals` | 
`public uint64_t total_tasks` | 
`public uint64_t completed_tasks` | 
`public uint64_t total_reports` | 
`public uint64_t approved_reports` | 
`public std::string meta` | 
`public inline  EOSLIB_SERIALIZE(`[`hosts`](#structhosts)`,(username)(registered_at)(architect)(hoperator)(type)(chat_mode)(consensus_percent)(referral_percent)(dacs_percent)(cfund_percent)(hfund_percent)(sys_percent)(levels)(gsponsor_model)(direct_goal_withdraw)(dac_mode)(total_dacs_weight)(ahost)(chosts)(sale_is_enabled)(sale_mode)(sale_token_contract)(asset_on_sale)(asset_on_sale_precision)(asset_on_sale_symbol)(sale_shift)(non_active_chost)(need_switch)(fhosts_mode)(fhosts)(title)(purpose)(voting_only_up)(power_market_id)(total_shares)(quote_amount)(quote_token_contract)(quote_symbol)(quote_precision)(root_token_contract)(root_token)(symbol)(precision)(to_pay)(payed)(cycle_start_id)(current_pool_id)(current_cycle_num)(current_pool_num)(parameters_setted)(activated)(priority_flag)(total_goals)(achieved_goals)(total_tasks)(completed_tasks)(total_reports)(approved_reports)(meta)) const` | 
`public inline eosio::name get_ahost() const` | 
`public inline eosio::symbol get_root_symbol() const` | 

## Members

### `public eosio::name username` {#structhosts_1a20938bba05a37320a8875a506d1d2ad1}





### `public eosio::time_point_sec registered_at` {#structhosts_1acd94c0fd8841fa120966a7ed2ede2e99}





### `public eosio::name architect` {#structhosts_1a7bbcbedf69f3a70bee5dca8da4f0f299}





### `public eosio::name hoperator` {#structhosts_1ae597139b6e779b34e63f5c0a78bd2675}





### `public eosio::name type` {#structhosts_1a5d51b449fc6701419c2e06b6045e1c79}





### `public eosio::name chat_mode` {#structhosts_1a7654231d14b15269cf881e0e181b041c}





### `public uint64_t consensus_percent` {#structhosts_1a6fb16cf97e6aa242fa89eeb4b7c5e70b}





### `public uint64_t referral_percent` {#structhosts_1a3cd22dae2f7d51f9e30ae3d3d5222c92}





### `public uint64_t dacs_percent` {#structhosts_1a4042ea164fa4913cfb00eeba3f237770}





### `public uint64_t cfund_percent` {#structhosts_1a5b1777c0a1e69811b2eb17830f65d73b}





### `public uint64_t hfund_percent` {#structhosts_1adccbeedcff76e76d268dd5309c62d306}





### `public uint64_t sys_percent` {#structhosts_1ad10dbf7c3e730b8b2d13045d5068062f}





### `public std::vector< uint64_t > levels` {#structhosts_1abdbbf47c6df55db5df5c85a4080bc219}





### `public std::vector< eosio::name > gsponsor_model` {#structhosts_1a254c752856c4b6443a3f45849a84f466}





### `public bool direct_goal_withdraw` {#structhosts_1abb50503adb4981cb11ebe099e9f024c3}





### `public uint64_t dac_mode` {#structhosts_1a93241a9165b6e6d37b12ffc0b56a2a00}





### `public uint64_t total_dacs_weight` {#structhosts_1aef8c1f7d03f391631abe95bf86ac1aa5}





### `public eosio::name ahost` {#structhosts_1a0afc2e8194f998c280b9a0f7ed373a48}





### `public std::vector< eosio::name > chosts` {#structhosts_1a9f5f508a3aa947262ac5d8891f197bef}





### `public bool sale_is_enabled` {#structhosts_1ae1af9662a7c4c35142db2901d72d393f}





### `public eosio::name sale_mode` {#structhosts_1a52683be3d7dd87b020ad2153b66d3579}





### `public eosio::name sale_token_contract` {#structhosts_1a1239eb9e569541798a8f1365ee544b30}





### `public eosio::asset asset_on_sale` {#structhosts_1a924bf2601605313ba15140160aa92db3}





### `public uint64_t asset_on_sale_precision` {#structhosts_1a1e53403d3c54fe5866eb0ab2b19e5ae1}





### `public std::string asset_on_sale_symbol` {#structhosts_1ae20109688575a0113d7cd2652e7a2033}





### `public int64_t sale_shift` {#structhosts_1a1bcbe02e4d50c85815bc03dcf72def70}





### `public bool non_active_chost` {#structhosts_1a9b7b35e8d7171a2d3ceada9b0fa17483}





### `public bool need_switch` {#structhosts_1a44a395cb6b8f15db069d2a42ae6837f7}





### `public uint64_t fhosts_mode` {#structhosts_1a438337e891336422d98bd74c97c03d7e}





### `public std::vector< eosio::name > fhosts` {#structhosts_1a6aaeaee4e3d5dea6cef4556837dac8cb}





### `public std::string title` {#structhosts_1a4eb9e6b68db5f7dfa5905b1a3cdb6942}





### `public std::string purpose` {#structhosts_1a2701d80e484eb9c7618a7bbb7b9aca94}





### `public bool voting_only_up` {#structhosts_1a121c2c8a6e032a358c61676ab69416e6}





### `public eosio::name power_market_id` {#structhosts_1a66e3d09a103f8ec55dee179bf152111c}





### `public uint64_t total_shares` {#structhosts_1abf00f55f83143e8a5a47bbed2bb85d0a}





### `public eosio::asset quote_amount` {#structhosts_1a063fd199cfd49c8e4bc3a513c991fe1b}





### `public eosio::name quote_token_contract` {#structhosts_1ad0543af217c9166111211df7e5ae4d10}





### `public std::string quote_symbol` {#structhosts_1ac89780b3abc69a5a15bd23e3f0a87636}





### `public uint64_t quote_precision` {#structhosts_1ab8357e7bc7d53e0234c7bb8fdd668dd3}





### `public eosio::name root_token_contract` {#structhosts_1a68dab42c2847909f51b03a1c97fbb3b6}





### `public eosio::asset root_token` {#structhosts_1ab25ffbf2b0edcccbc4e0a9cd5f9cd6b5}





### `public std::string symbol` {#structhosts_1a17c085f318a91d470a0f7b32e8c36dcd}





### `public uint64_t precision` {#structhosts_1ac235bddf1a3a4ec7b326fdaf0539728f}





### `public eosio::asset to_pay` {#structhosts_1a4692b62c2bf0a73bfc98fd3b3e20cfb1}





### `public bool payed` {#structhosts_1a8256089a682ad0c46f9deca1478450e1}





### `public uint64_t cycle_start_id` {#structhosts_1a18b42ecfa193db770fa4226dd6e82b0d}





### `public uint64_t current_pool_id` {#structhosts_1a2d82a264c4140047d7c1910c5688e433}





### `public uint64_t current_cycle_num` {#structhosts_1a0bffc7f8877a3c5d8e21d9d427ce2cd1}





### `public uint64_t current_pool_num` {#structhosts_1a703c5a487e62f539ada0d0919c6d4d8a}





### `public bool parameters_setted` {#structhosts_1a0477b593cb8d61af22ce82412f4fed2b}





### `public bool activated` {#structhosts_1ab614f55d27e39c67fef74656c9b9ef6d}





### `public bool priority_flag` {#structhosts_1a9ccdcc8e52f110b73bfe268c2974a4de}





### `public uint64_t total_goals` {#structhosts_1ad07e72527df764f0f8af057383e70779}





### `public uint64_t achieved_goals` {#structhosts_1adbea0c2fde59423cbcca98fe36f560d3}





### `public uint64_t total_tasks` {#structhosts_1a6e0bb8abc121c4713d0fed50bc6dde0b}





### `public uint64_t completed_tasks` {#structhosts_1ab4622f3ad8104be5680d5241244b0592}





### `public uint64_t total_reports` {#structhosts_1a3aea471591de79d52ef50ff662ee3cab}





### `public uint64_t approved_reports` {#structhosts_1aba324f4fd3d8c36615e66cf74d3b6729}





### `public std::string meta` {#structhosts_1a5c7cc5c65f7e7c4f6eaebf34fbb83c2e}





### `public inline  EOSLIB_SERIALIZE(`[`hosts`](#structhosts)`,(username)(registered_at)(architect)(hoperator)(type)(chat_mode)(consensus_percent)(referral_percent)(dacs_percent)(cfund_percent)(hfund_percent)(sys_percent)(levels)(gsponsor_model)(direct_goal_withdraw)(dac_mode)(total_dacs_weight)(ahost)(chosts)(sale_is_enabled)(sale_mode)(sale_token_contract)(asset_on_sale)(asset_on_sale_precision)(asset_on_sale_symbol)(sale_shift)(non_active_chost)(need_switch)(fhosts_mode)(fhosts)(title)(purpose)(voting_only_up)(power_market_id)(total_shares)(quote_amount)(quote_token_contract)(quote_symbol)(quote_precision)(root_token_contract)(root_token)(symbol)(precision)(to_pay)(payed)(cycle_start_id)(current_pool_id)(current_cycle_num)(current_pool_num)(parameters_setted)(activated)(priority_flag)(total_goals)(achieved_goals)(total_tasks)(completed_tasks)(total_reports)(approved_reports)(meta)) const` {#structhosts_1a5134c37a37ee240d75cb5f0fa36fb939}





### `public inline eosio::name get_ahost() const` {#structhosts_1aaeac4196f2bcd3a2826acbaea5938e85}





### `public inline eosio::symbol get_root_symbol() const` {#structhosts_1ad8c619ace703952a8ae1cdbe1217814e}






# struct `hosts2` {#structhosts2}


Расширение структуры хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | 
`public eosio::time_point_sec sale_price_updated_at` | 
`public uint64_t sale_price` | 
`public inline  EOSLIB_SERIALIZE(`[`hosts2`](#structhosts2)`,(username)(sale_price_updated_at)(sale_price)) const` | 

## Members

### `public eosio::name username` {#structhosts2_1abe43d4f81e560ed0d064e37f72ea29aa}





### `public eosio::time_point_sec sale_price_updated_at` {#structhosts2_1a0b5d9184976b6386bbd926d7099d69ac}





### `public uint64_t sale_price` {#structhosts2_1ada9a20ae223bc40d035d4b89c3b0f155}





### `public inline  EOSLIB_SERIALIZE(`[`hosts2`](#structhosts2)`,(username)(sale_price_updated_at)(sale_price)) const` {#structhosts2_1a68dbdf967c6fc56ac256b0d6369b2e5c}






# struct `hostsonfunds` {#structhostsonfunds}


Структура хостов Двойной Спирали, подключенных к глобальным фондам распределения.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t fund_id` | 
`public eosio::name host` | 
`public inline uint64_t primary_key() const` | 
`public inline uint128_t fundandhost() const` | 

## Members

### `public uint64_t id` {#structhostsonfunds_1ae08cae788f418dbd02d0b3d482cd550e}





### `public uint64_t fund_id` {#structhostsonfunds_1aeba4b7f5a7bb81d2d3a43ee7d05c00ed}





### `public eosio::name host` {#structhostsonfunds_1a9c534a08820e7da9a26ac70ff3b2ee43}





### `public inline uint64_t primary_key() const` {#structhostsonfunds_1ad215eeef6cec763f1ab292ea6c29112f}





### `public inline uint128_t fundandhost() const` {#structhostsonfunds_1aa6f6be0c760701a5f38affb00713d253}






# struct `incoming` {#structincoming}


Структура задач, где аккаунт деятель.

Структура входящих целей и задач

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name host` | 
`public uint64_t goal_id` | 
`public uint64_t task_id` | 
`public bool with_badge` | 
`public uint64_t my_goal_id` | 
`public uint64_t my_badge_id` | 
`public inline uint64_t primary_key() const` | 
`public inline uint128_t byhosttask() const` | 
`public inline uint128_t byhostgoal() const` | 
`public inline uint64_t bymygoal() const` | 

## Members

### `public uint64_t id` {#structincoming_1a9ee49a4eb869acf3122ebe8285cb4194}





### `public eosio::name host` {#structincoming_1a242c3f43114c14b76ec2e7c63d8c231a}





### `public uint64_t goal_id` {#structincoming_1ade1b23166cc5f00c5671a4eeb15acd4c}





### `public uint64_t task_id` {#structincoming_1a3380ef481a49b4516746460ec43c0ed0}





### `public bool with_badge` {#structincoming_1a0389fa468d16a51288e568a0eec33e5d}





### `public uint64_t my_goal_id` {#structincoming_1a7da14ab3595bd9ddf804e2dfc52513ba}





### `public uint64_t my_badge_id` {#structincoming_1a157d02a8368b540707e942e22d1905e9}





### `public inline uint64_t primary_key() const` {#structincoming_1ae9c10bc16472d43fb3ccb4d92ceafe09}





### `public inline uint128_t byhosttask() const` {#structincoming_1aa1299c15e18b2512be11f58769a5a5da}





### `public inline uint128_t byhostgoal() const` {#structincoming_1ad317d70b568a875fa44fabff231421e9}





### `public inline uint64_t bymygoal() const` {#structincoming_1a2e9ca9eb34ff5437c38d36fc4f98a7ec}






# struct `partners2` {#structpartners2}


Структура партнёров и их партнёров.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | 
`public eosio::name referer` | 
`public std::string nickname` | 
`public eosio::checksum256 nickhash` | 
`public uint64_t id` | 
`public uint64_t cashback` | 
`public eosio::name status` | 
`public std::string meta` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byreferer() const` | 
`public inline uint64_t byid() const` | 
`public inline uint64_t bystatus() const` | 
`public inline eosio::checksum256 bynickhash() const` | 

## Members

### `public eosio::name username` {#structpartners2_1a7b279bce1d671ae29aa0d9b14936204b}





### `public eosio::name referer` {#structpartners2_1a6198ca816832f4fcb8b6211c9e88f7b1}





### `public std::string nickname` {#structpartners2_1adb751d90a64a4b19f0aab7e61f792f25}





### `public eosio::checksum256 nickhash` {#structpartners2_1aadb1fde1e890fa68678cba34acb50b82}





### `public uint64_t id` {#structpartners2_1a8902d55d8abeecef589daabb3c0e85e5}





### `public uint64_t cashback` {#structpartners2_1af916875cf91b9cd8534f0cbe84317a6c}





### `public eosio::name status` {#structpartners2_1aa9d5cb6acaacac876169c5f3efddcf1e}





### `public std::string meta` {#structpartners2_1a8189bf00e09ffc820b76f99acd7b337a}





### `public inline uint64_t primary_key() const` {#structpartners2_1a37a7250a0195969762903785a65b1b8b}





### `public inline uint64_t byreferer() const` {#structpartners2_1a208a5ac23d7db6fa02449eea1f6322d2}





### `public inline uint64_t byid() const` {#structpartners2_1a92d13db109533760e4bc935062e62b56}





### `public inline uint64_t bystatus() const` {#structpartners2_1a379e47f8474351bfafe91231cb471826}





### `public inline eosio::checksum256 bynickhash() const` {#structpartners2_1afa8611fed050f191719f0e4fbaf713a8}






# struct `plog` {#structplog}


Структура истории преобретения силы пользователем у хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name host` | 
`public uint64_t pool_id` | 
`public uint64_t cycle_num` | 
`public uint64_t pool_num` | 
`public uint64_t power` | 
`public inline uint64_t primary_key() const` | 
`public inline uint128_t hostpoolid() const` | 

## Members

### `public uint64_t id` {#structplog_1a501a38bc5d48aa3a5d879cdcc104e40d}





### `public eosio::name host` {#structplog_1a9879ade8388b63b3b09e97f7adfd5df2}





### `public uint64_t pool_id` {#structplog_1ac7f6ba0c58f77cfdbc117db6dc7ed2cc}





### `public uint64_t cycle_num` {#structplog_1aa71a29d3e3781cd6a75b08c259cb347a}





### `public uint64_t pool_num` {#structplog_1a542d69ec24dac22598bd2638bf13b4ca}





### `public uint64_t power` {#structplog_1a024b8605bc0c7808115d7d2510b74705}





### `public inline uint64_t primary_key() const` {#structplog_1a8902d07feb1ca31a509e89362e5834e3}





### `public inline uint128_t hostpoolid() const` {#structplog_1a6e5d2385d3e1fb9c8f9d220e2d17be61}






# struct `pool` {#structpool}


Структура для учёта распределения бассейнов внутренней учетной единицы хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name ahost` | 
`public uint64_t cycle_num` | 
`public uint64_t pool_num` | 
`public std::string color` | 
`public uint64_t total_quants` | 
`public uint64_t remain_quants` | 
`public uint64_t creserved_quants` | 
`public eosio::asset filled` | 
`public eosio::asset remain` | 
`public uint64_t filled_percent` | 
`public eosio::asset pool_cost` | 
`public eosio::asset quant_cost` | 
`public eosio::asset total_win_withdraw` | 
`public eosio::asset total_loss_withdraw` | 
`public eosio::time_point_sec pool_started_at` | 
`public eosio::time_point_sec priority_until` | 
`public eosio::time_point_sec pool_expired_at` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bycycle() const` | 

## Members

### `public uint64_t id` {#structpool_1a845730fab790f597c89eae77c4e55cd1}





### `public eosio::name ahost` {#structpool_1a8d459a6a3b3df8bcc78c9f706dbe6a7a}





### `public uint64_t cycle_num` {#structpool_1a6992bd5dbc660852de66f5318bba71fe}





### `public uint64_t pool_num` {#structpool_1afd74b755e6bffa6862bbee4b6dab1278}





### `public std::string color` {#structpool_1a32f3da84316e46cff007d3f78554e3eb}





### `public uint64_t total_quants` {#structpool_1a84312b9a0d0362156a1e46f539f1d89f}





### `public uint64_t remain_quants` {#structpool_1aa436a133fa79a31afc168ff30253e629}





### `public uint64_t creserved_quants` {#structpool_1a1fbce6aae1ac1f390ba521b3b0ff05c5}





### `public eosio::asset filled` {#structpool_1ae307ede011f908480b42c32226035684}





### `public eosio::asset remain` {#structpool_1a8830ee5b935cd6e19675bbc14742d4a2}





### `public uint64_t filled_percent` {#structpool_1ae3fef506dbed519a6ccd2687ae4d54d0}





### `public eosio::asset pool_cost` {#structpool_1a20f73c48bfdadd281bdb6c5726b7d495}





### `public eosio::asset quant_cost` {#structpool_1a0cab32829724758ea9261e7c36e08339}





### `public eosio::asset total_win_withdraw` {#structpool_1a72ce0339159c648358fb53a32a8c4c73}





### `public eosio::asset total_loss_withdraw` {#structpool_1a6ff847b127b7af06682f39c3ea147a59}





### `public eosio::time_point_sec pool_started_at` {#structpool_1afc0e41c1d7352dccf1988b31e5cdd7a1}





### `public eosio::time_point_sec priority_until` {#structpool_1a91a910784f3c11a8941fd3c0afd9e9f2}





### `public eosio::time_point_sec pool_expired_at` {#structpool_1a86f9ea628d7ee5ce4bc4e38f79d01b4b}





### `public inline uint64_t primary_key() const` {#structpool_1a6ff21f4b3d828f6be2ab5a8214d05e47}





### `public inline uint64_t bycycle() const` {#structpool_1ad4ba2fac7edc59ef01fe0ec3c8fb9187}






# struct `power` {#structpower}


Структура силы пользователя у хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name host` | 
`public uint64_t power` | 
`public uint64_t staked` | 
`public uint64_t delegated` | 
`public uint64_t with_badges` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name host` {#structpower_1ab77f28af76fdd70f9b74eeed48d54870}





### `public uint64_t power` {#structpower_1ae3806eb0de73dd7ad6d7aea781238236}





### `public uint64_t staked` {#structpower_1afc77ca7b22e5a62402eada2a2b237b96}





### `public uint64_t delegated` {#structpower_1a29ae18db4785b901524f580bc470914c}





### `public uint64_t with_badges` {#structpower_1a01f487ee3cc0de104701ae9bd3a5549f}





### `public inline uint64_t primary_key() const` {#structpower_1a861dcae256146b2b675f9eaf7c76556e}






# struct `power2` {#structpower2}


Расширение структуры силы пользователя у хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name host` | 
`public uint64_t frozen` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name host` {#structpower2_1ab2ac79460fae527f88c8a70a24cf23e9}





### `public uint64_t frozen` {#structpower2_1a1f6545b2cf1eeac414b618c48cad89cb}





### `public inline uint64_t primary_key() const` {#structpower2_1a19e385a354dbb08dff7385cc4a2da457}






# struct `power3` {#structpower3}


Обновленная структура силы пользователя у хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | 
`public uint64_t power` | 
`public uint64_t staked` | 
`public uint64_t delegated` | 
`public uint64_t with_badges` | 
`public uint64_t frozen` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bypower() const` | 
`public inline uint64_t bystaked() const` | 
`public inline uint64_t bydelegated() const` | 
`public inline uint64_t bywbadges() const` | 
`public inline uint64_t byfrozen() const` | 

## Members

### `public eosio::name username` {#structpower3_1a1a62921d753598e4bc97e1f900e57da7}





### `public uint64_t power` {#structpower3_1a6b7f01357d49db2faaa4ec39783e6dda}





### `public uint64_t staked` {#structpower3_1a2787d6a09d20a84b2874bfe9a5c18228}





### `public uint64_t delegated` {#structpower3_1a99f2d6242441cc47bb42c06f7899d47f}





### `public uint64_t with_badges` {#structpower3_1ae6d8dfc19ca72b4578bc19ce64e008ec}





### `public uint64_t frozen` {#structpower3_1a85e7e94bb9405f2d65665a6c2e584329}





### `public inline uint64_t primary_key() const` {#structpower3_1aa06e6c5f25d11e5a76d18ee51ab464fd}





### `public inline uint64_t bypower() const` {#structpower3_1a00fc4fcf68fcccd63bab250ba3064874}





### `public inline uint64_t bystaked() const` {#structpower3_1a323807599f888d02878526568232990e}





### `public inline uint64_t bydelegated() const` {#structpower3_1a2ba9d33f4c24813e3888cf8d91049ade}





### `public inline uint64_t bywbadges() const` {#structpower3_1a708ed95e50528c284fb8beb1679102ce}





### `public inline uint64_t byfrozen() const` {#structpower3_1a68c582634031ca9a7b9a6cd3a8f10405}






# struct `producer_info` {#structproducer__info}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name owner` | 
`public double total_votes` | 
`public eosio::public_key producer_key` | 
`public bool is_active` | a packed public key object
`public std::string url` | 
`public uint32_t unpaid_blocks` | 
`public time_point last_claim_time` | 
`public uint16_t location` | 
`public inline uint64_t primary_key() const` | 
`public inline double by_votes() const` | 
`public inline bool active() const` | 
`public inline void deactivate()` | 

## Members

### `public name owner` {#structproducer__info_1afcb44cafef73930c9c8fd31e5fea8333}





### `public double total_votes` {#structproducer__info_1a0635bce8d0653bbecdfe113a4b659a56}





### `public eosio::public_key producer_key` {#structproducer__info_1ae3341e1516eef67ab1a3524bdd1dc03b}





### `public bool is_active` {#structproducer__info_1a1064dde6b317361ddf7c61b65f7687e7}

a packed public key object



### `public std::string url` {#structproducer__info_1a1f796468dfe9604ed313eda920a28c00}





### `public uint32_t unpaid_blocks` {#structproducer__info_1a61a82e8dedfd7dc33d7e8395cc75a765}





### `public time_point last_claim_time` {#structproducer__info_1ac65e5d398ed775a2655da08ad130db43}





### `public uint16_t location` {#structproducer__info_1a459371049ff8d064ec0d1055e7620a22}





### `public inline uint64_t primary_key() const` {#structproducer__info_1a123b853ad6eaac551d206fd1eacb98b6}





### `public inline double by_votes() const` {#structproducer__info_1a864791e030cd80873c2f2f76dcf9dac1}





### `public inline bool active() const` {#structproducer__info_1a2a3b1632bbd8412ef28cfe746c580b1b}





### `public inline void deactivate()` {#structproducer__info_1aead564e246d81b1deec752ac734f523a}






# struct `pstats` {#structpstats}


Структура статистики распределения безусловного потока жетонов хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name host` | 
`public eosio::asset total_available_in_asset` | 
`public uint64_t pflow_last_withdrawed_pool_id` | 
`public uint128_t pflow_available_segments` | 
`public eosio::asset pflow_available_in_asset` | 
`public eosio::asset pflow_withdrawed` | 
`public eosio::asset ref_available_in_asset` | 
`public uint64_t ref_available_segments` | 
`public eosio::asset ref_withdrawed` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name host` {#structpstats_1ab4cf368b746c511fcd22ca33f96126c7}





### `public eosio::asset total_available_in_asset` {#structpstats_1a75e7a460cbd3bb65d4dd20c8448f612f}





### `public uint64_t pflow_last_withdrawed_pool_id` {#structpstats_1a3de2abae9166d7a70adde273169890e0}





### `public uint128_t pflow_available_segments` {#structpstats_1a3f5ad972e496774a7d0c46be6e5148f0}





### `public eosio::asset pflow_available_in_asset` {#structpstats_1aa43e842556a658e53e6a69f26c230cf4}





### `public eosio::asset pflow_withdrawed` {#structpstats_1a8ade8604d05f157631139dc455766572}





### `public eosio::asset ref_available_in_asset` {#structpstats_1a917e1e83e48f79abec8c040e4e07a83a}





### `public uint64_t ref_available_segments` {#structpstats_1aa58b0a4588cd02e7360bf21bdd5340e4}





### `public eosio::asset ref_withdrawed` {#structpstats_1af3c5983d5ed3389982e516d84c84eaa1}





### `public inline uint64_t primary_key() const` {#structpstats_1a6ec34c82ee16baa47636693356b443d8}






# struct `rate` {#structrate}


Структура курсов реализации внутренней конвертационной единицы и их возврата Протоколу.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t pool_id` | 
`public uint64_t buy_rate` | 
`public uint64_t sell_rate` | 
`public uint64_t convert_rate` | 
`public eosio::asset quant_buy_rate` | 
`public eosio::asset quant_sell_rate` | 
`public eosio::asset quant_convert_rate` | 
`public eosio::asset client_income` | 
`public eosio::asset delta` | 
`public eosio::asset pool_cost` | 
`public eosio::asset total_in_box` | 
`public eosio::asset payment_to_wins` | 
`public eosio::asset payment_to_loss` | 
`public eosio::asset system_income` | 
`public eosio::asset live_balance_for_sale` | 
`public eosio::asset live_balance_for_convert` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t pool_id` {#structrate_1a2b87f5a0629a455443d784ccbf84287c}





### `public uint64_t buy_rate` {#structrate_1a2267ea65cd8990c87426ba3091e95e85}





### `public uint64_t sell_rate` {#structrate_1a6e21a3a6d869ec8b87c54cb4ee2ad375}





### `public uint64_t convert_rate` {#structrate_1ac9f9ea66d77c2cec08b1f5f8676c58c1}





### `public eosio::asset quant_buy_rate` {#structrate_1a46bc251e2c71cfc2258505fe2461c7e4}





### `public eosio::asset quant_sell_rate` {#structrate_1a5bf20ca9d09de3efc8a583a2235ef58c}





### `public eosio::asset quant_convert_rate` {#structrate_1a4bd943c561ccabc0db2e90019f2120c7}





### `public eosio::asset client_income` {#structrate_1ae062e16cbc4a957633c87db9481d4e57}





### `public eosio::asset delta` {#structrate_1a577cd2278a61dda70efc479e6bfdd702}





### `public eosio::asset pool_cost` {#structrate_1a71141faf60e9e01b298330ac9ee566b7}





### `public eosio::asset total_in_box` {#structrate_1afbbb65641251ea5537628c007a5412a1}





### `public eosio::asset payment_to_wins` {#structrate_1a8139680b8f6fe6087e669f6aafa16570}





### `public eosio::asset payment_to_loss` {#structrate_1acfb02bea7cf708ddb76f0d2a40cfdc37}





### `public eosio::asset system_income` {#structrate_1a2a8e6ec3756b3db73f6113117d2e47f9}





### `public eosio::asset live_balance_for_sale` {#structrate_1a0187445eb6729f79be32af3af174a87f}





### `public eosio::asset live_balance_for_convert` {#structrate_1ae65d9d51957c5ed2eac024f2371a475e}





### `public inline uint64_t primary_key() const` {#structrate_1a72d440ee345e18b0436f4aaf5053a453}






# struct `refbalances` {#structrefbalances}


Структура полученных реферальных балансов от партнёров на партнёра.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name host` | 
`public eosio::time_point_sec timepoint_sec` | 
`public eosio::asset refs_amount` | 
`public eosio::asset win_amount` | 
`public eosio::asset amount` | 
`public eosio::name from` | 
`public uint64_t cashback` | 
`public uint8_t level` | 
`public uint64_t percent` | 
`public double segments` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structrefbalances_1a34a8a9d5c56b24de0829d4a9d81c4269}





### `public eosio::name host` {#structrefbalances_1a0d55b785c080f4d6dc43b61e989b36ae}





### `public eosio::time_point_sec timepoint_sec` {#structrefbalances_1a760220fca952e44324bec6c283d78508}





### `public eosio::asset refs_amount` {#structrefbalances_1adb665594bda054dc09fb0a19ce7c624e}





### `public eosio::asset win_amount` {#structrefbalances_1ac23a0a9c62f64db74bd819d837e21508}





### `public eosio::asset amount` {#structrefbalances_1a2a19cbf0c5c081f639f1d49f9015d9da}





### `public eosio::name from` {#structrefbalances_1a4b7c4278ee25e6b06217d6344e492811}





### `public uint64_t cashback` {#structrefbalances_1a4d987f9d8ae123989bd1b24c03452f5e}





### `public uint8_t level` {#structrefbalances_1aa19c983f2588d068b82d315d45704f10}





### `public uint64_t percent` {#structrefbalances_1a9a1457ff6e68a73c515e3ac5ab61016b}





### `public double segments` {#structrefbalances_1ab1acdfa60be29ef313f41b864539f98c}





### `public inline uint64_t primary_key() const` {#structrefbalances_1aa302faf949bc790abf7f83c3d716942f}






# struct `reports3` {#structreports3}


Структура отчетов по задачам хоста Двойной Спирали.

Структура отчетов по задачам хоста Двойной Спирали.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t report_id` | 
`public eosio::name status` | 
`public uint64_t task_id` | 
`public uint64_t goal_id` | 
`public uint64_t type` | 
`public uint64_t count` | 
`public eosio::name username` | 
`public eosio::name curator` | 
`public eosio::string data` | 
`public eosio::asset requested` | 
`public eosio::asset balance` | 
`public eosio::asset withdrawed` | 
`public bool need_check` | 
`public bool approved` | 
`public bool distributed` | 
`public eosio::string comment` | 
`public eosio::time_point_sec created_at` | 
`public eosio::time_point_sec expired_at` | 
`public uint64_t total_votes` | 
`public std::vector< eosio::name > voters` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byusername() const` | 
`public inline uint64_t bytaskid() const` | 
`public inline uint64_t bygoalid() const` | 
`public inline uint64_t byvotes() const` | 
`public inline uint128_t userwithgoal() const` | 
`public inline uint128_t userwithtask() const` | 

## Members

### `public uint64_t report_id` {#structreports3_1a5b95f577b1824aefa16eb0a4ecb3a959}





### `public eosio::name status` {#structreports3_1af16a63bc34bf5e07b88f92032808057a}





### `public uint64_t task_id` {#structreports3_1a081eedcabc34fc9555fcab1adf4550cf}





### `public uint64_t goal_id` {#structreports3_1a85ecdc2d76cf4f3065a02945e542807f}





### `public uint64_t type` {#structreports3_1a649e983b78278a792ae960440527122f}





### `public uint64_t count` {#structreports3_1aeab090f34f040bad2642c793e8231328}





### `public eosio::name username` {#structreports3_1ad863844027d367c8f8d92c467797e1ea}





### `public eosio::name curator` {#structreports3_1ab0008d05f3d601b30ca2db4420ab32f5}





### `public eosio::string data` {#structreports3_1ad70c9e74c31a68bef31e68c4971f2614}





### `public eosio::asset requested` {#structreports3_1a8b4a94d6501522c2d25356274de01ef8}





### `public eosio::asset balance` {#structreports3_1abcfc36d73b59d2c8a9f66feb191df5f7}





### `public eosio::asset withdrawed` {#structreports3_1aff965e010c116551b43d97b9e402994f}





### `public bool need_check` {#structreports3_1a219b0d07e6707bde21acbec7d9b49dd7}





### `public bool approved` {#structreports3_1a3efe232c9e28e3bdaeabd6713204a88c}





### `public bool distributed` {#structreports3_1a0618148885e6db5d37751e1318ce2b1c}





### `public eosio::string comment` {#structreports3_1a71e900f1eb1b73cee55c4cdb0d80c027}





### `public eosio::time_point_sec created_at` {#structreports3_1a825f77becac6fd1df509fedcf4ac910e}





### `public eosio::time_point_sec expired_at` {#structreports3_1ae7cb23c8867ab37df6b257c9e5c43033}





### `public uint64_t total_votes` {#structreports3_1a749104ff4d8f14a4cdb1e5eada51cf2d}





### `public std::vector< eosio::name > voters` {#structreports3_1ad17e01089c5b3d67155663e56a5d58bb}





### `public inline uint64_t primary_key() const` {#structreports3_1ae691681169805ec57fd02ca779910ed8}





### `public inline uint64_t byusername() const` {#structreports3_1a52ab0b2d834ca584b53975e93542f971}





### `public inline uint64_t bytaskid() const` {#structreports3_1a38d535110bebc76376c8d952b18a6186}





### `public inline uint64_t bygoalid() const` {#structreports3_1afd6b06bd9d6a1575f6d37829b132469b}





### `public inline uint64_t byvotes() const` {#structreports3_1aa654e06d991a5e57e3b8e7928c0eed98}





### `public inline uint128_t userwithgoal() const` {#structreports3_1ae6b51354e281bfc71d174fb207699afa}





### `public inline uint128_t userwithtask() const` {#structreports3_1a0944b5d06f8d2c9449dcf7d04f0534e1}






# struct `roles` {#structroles}


Структура командных ролей протокола.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t role_id` | 
`public eosio::name model` | 
`public eosio::name lang` | 
`public std::string title` | 
`public std::string descriptor` | 
`public eosio::name suggester` | 
`public bool approved` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t role_id` {#structroles_1a09f3a5f9d5f49dc5f42dd341c3625921}





### `public eosio::name model` {#structroles_1a1636a96590f017868b52e4fb0981b122}





### `public eosio::name lang` {#structroles_1ad862fbb4086a677484e0b08e44c46b4b}





### `public std::string title` {#structroles_1af0b8c91fa6b8352837b7dbcec1ce1103}





### `public std::string descriptor` {#structroles_1ac00f8989da077196f53ef6b114d203f0}





### `public eosio::name suggester` {#structroles_1a386104c4d56af816a3c4c486c521666b}





### `public bool approved` {#structroles_1ac5d3a4e7af37f855dc3c7772794d7cdb}





### `public inline uint64_t primary_key() const` {#structroles_1a77e285402e8f705d872e8f972d60da5d}






# struct `rstat` {#structrstat}


Структура статистики реферальных балансов и осадок, доступный на получение по мере накопления.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name host` | 
`public eosio::asset withdrawed` | 
`public double sediment` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name host` {#structrstat_1ad16be0649bc5fc500887b1599c8897df}





### `public eosio::asset withdrawed` {#structrstat_1ae1099534688656a0f758093c41fc7e52}





### `public double sediment` {#structrstat_1a29f284b73dc1a003686498e32f2730b7}





### `public inline uint64_t primary_key() const` {#structrstat_1af94b16bcf555b0016b8a0f6c25505031}






# struct `rvotes` {#structrvotes}


Структура голосов за отчёты



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t task_id` | 
`public uint64_t report_id` | 
`public eosio::name host` | 
`public int64_t power` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byhost() const` | 
`public inline uint128_t idwithhost() const` | 
`public inline uint64_t bytaskid() const` | 

## Members

### `public uint64_t id` {#structrvotes_1a322bf9b1d0e5489d26b6b24a13532722}





### `public uint64_t task_id` {#structrvotes_1a6b175e7d84f8aac98675102b07ae93fe}





### `public uint64_t report_id` {#structrvotes_1a854dbd32980b0bf64c82bf499a80c2a1}





### `public eosio::name host` {#structrvotes_1af867680150bd09153f10fe2831687b8c}





### `public int64_t power` {#structrvotes_1ad8590180d851272c4712165aa9b30883}





### `public inline uint64_t primary_key() const` {#structrvotes_1a7b9563ac61083fbdc75bec7c955c6fc5}





### `public inline uint64_t byhost() const` {#structrvotes_1a8c0d5e2570290b890412846556c2a494}





### `public inline uint128_t idwithhost() const` {#structrvotes_1a31893f25a092c6b9a70919f547b51107}





### `public inline uint64_t bytaskid() const` {#structrvotes_1ac76fa641e0af8e6d2f4664d7df4d6fc1}






# struct `sale` {#structsale}


Структура истории движения жетона распределения хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t pool_id` | 
`public uint64_t sell_rate` | 
`public eosio::asset solded` | 
`public eosio::asset total_solded` | 
`public inline uint64_t primary_key() const` | 
`public  EOSLIB_SERIALIZE(`[`sale`](#structsale)`,(pool_id)(sell_rate)(solded)(total_solded))` | 

## Members

### `public uint64_t pool_id` {#structsale_1a9a9e29e910e7fad4176920b4c5820598}





### `public uint64_t sell_rate` {#structsale_1adcf1a6b39d7e7a342974ae39d90a4126}





### `public eosio::asset solded` {#structsale_1a75f7c022bad8909393daa51e86c970fc}





### `public eosio::asset total_solded` {#structsale_1af1093f1b1348b3a9a91362f3228ee503}





### `public inline uint64_t primary_key() const` {#structsale_1ab961633ac6594ad31b8862f2847702f5}





### `public  EOSLIB_SERIALIZE(`[`sale`](#structsale)`,(pool_id)(sell_rate)(solded)(total_solded))` {#structsale_1a5d93f15e70be03416e1a2ffa19c58748}






# struct `sincome` {#structsincome}


Структура учёта системного дохода фондов хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t pool_id` | 
`public eosio::name ahost` | 
`public uint64_t cycle_num` | 
`public uint64_t pool_num` | 
`public uint64_t liquid_power` | 
`public eosio::asset max` | 
`public eosio::asset total` | 
`public eosio::asset paid_to_refs` | 
`public eosio::asset paid_to_dacs` | 
`public eosio::asset paid_to_cfund` | 
`public eosio::asset paid_to_hfund` | 
`public eosio::asset paid_to_sys` | 
`public uint128_t hfund_in_segments` | 
`public uint128_t distributed_segments` | 
`public inline uint64_t primary_key() const` | 
`public inline uint128_t cyclandpool() const` | 

## Members

### `public uint64_t pool_id` {#structsincome_1a09ece84fa00731047836986ca1786ef8}





### `public eosio::name ahost` {#structsincome_1a2d0d6337a359acf0734c96697b348f47}





### `public uint64_t cycle_num` {#structsincome_1a876a7e7ebf08639f7ad88e67fb04b71d}





### `public uint64_t pool_num` {#structsincome_1acb00f29572d26e42f130437d68d818b6}





### `public uint64_t liquid_power` {#structsincome_1a2273801c23b8d1d29bfe4993c545310c}





### `public eosio::asset max` {#structsincome_1a235cda95c9ef83298fc9a7aeccc54446}





### `public eosio::asset total` {#structsincome_1ae55609215cedb51a2c9e76fb32c79d58}





### `public eosio::asset paid_to_refs` {#structsincome_1add329640eaf3fc690232466e75cb8671}





### `public eosio::asset paid_to_dacs` {#structsincome_1a422ee3f187ebb2f8bdc9498b424179da}





### `public eosio::asset paid_to_cfund` {#structsincome_1af28f3d60a447ccebbf167ed976f78f89}





### `public eosio::asset paid_to_hfund` {#structsincome_1a2f295c491b230cf0fba00f94478df106}





### `public eosio::asset paid_to_sys` {#structsincome_1a6ac53154170813103756183c5d74e86b}





### `public uint128_t hfund_in_segments` {#structsincome_1a4092658b524a05c672fa7411bfcf8b24}





### `public uint128_t distributed_segments` {#structsincome_1a34153b6476a8efdb0e2edf8ff7890628}





### `public inline uint64_t primary_key() const` {#structsincome_1acfcdc82a654a7934f86c8e9f778a3860}





### `public inline uint128_t cyclandpool() const` {#structsincome_1afafc00e90214c947e7905d360c99a3cf}






# struct `sincome2` {#structsincome2}


Доп структура учёта системного дохода фондов хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t pool_id` | 
`public eosio::asset burned_loss_amount` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t pool_id` {#structsincome2_1a2fc56b3db567d9dbee93795cfc0621df}





### `public eosio::asset burned_loss_amount` {#structsincome2_1a562248e01d10f7967f6f8eeb069f4614}





### `public inline uint64_t primary_key() const` {#structsincome2_1a4894afa5216d13f6fec791f3e3bf1d88}






# struct `spiral` {#structspiral}


Структура основных параметров конфигурации Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t size_of_pool` | 
`public uint64_t quants_precision` | 
`public uint64_t overlap` | 
`public uint64_t profit_growth` | 
`public uint64_t base_rate` | 
`public uint64_t loss_percent` | 
`public uint64_t pool_limit` | 
`public uint64_t pool_timeout` | 
`public uint64_t priority_seconds` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structspiral_1a821f26f81cb47836d12f2d9d6766becc}





### `public uint64_t size_of_pool` {#structspiral_1a7e915995337434e5384c4a77608f16c8}





### `public uint64_t quants_precision` {#structspiral_1adeb786a290477f8b09bdc20250262729}





### `public uint64_t overlap` {#structspiral_1a89967c6e844fba55fe1b2374a835541f}





### `public uint64_t profit_growth` {#structspiral_1adcd22c1cbd48c166b4634d17cae78075}





### `public uint64_t base_rate` {#structspiral_1a79273adc97e3203294c9be82c6302f03}





### `public uint64_t loss_percent` {#structspiral_1aa38a1701402e7f10ad1dac35cd8acef1}





### `public uint64_t pool_limit` {#structspiral_1a7178955a5b0e1efcb250d6093e7d15da}





### `public uint64_t pool_timeout` {#structspiral_1a112e2db48fc525b87553bb3892d27b74}





### `public uint64_t priority_seconds` {#structspiral_1a46399788ec0c731594a2b57fa2389a20}





### `public inline uint64_t primary_key() const` {#structspiral_1afaf00204ab791522487a8498110136e3}






# struct `spiral2` {#structspiral2}


Структура компенсационных параметров конфигурации Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t compensator_percent` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structspiral2_1a8350391d8b0d686ae7e11c7706b4d3d6}





### `public uint64_t compensator_percent` {#structspiral2_1a9266c6802c921a3b7006ddc9aa003b29}





### `public inline uint64_t primary_key() const` {#structspiral2_1a0432f80414e4f3ec3b85c50ac335f3ff}






# struct `tasks` {#structtasks}


Структура задач хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t task_id` | 
`public uint64_t goal_id` | 
`public eosio::name host` | 
`public eosio::name creator` | 
`public eosio::name benefactor` | 
`public eosio::name suggester` | 
`public std::string permlink` | 
`public eosio::name type` | 
`public eosio::name status` | 
`public uint64_t priority` | 
`public bool is_regular` | 
`public std::vector< uint64_t > calendar` | 
`public bool is_public` | 
`public eosio::name doer` | 
`public eosio::name role` | 
`public uint64_t level` | 
`public eosio::string title` | 
`public eosio::string data` | 
`public eosio::asset requested` | 
`public eosio::asset funded` | 
`public eosio::asset remain` | 
`public eosio::asset for_each` | 
`public eosio::name curator` | 
`public uint64_t gifted_badges` | 
`public uint64_t gifted_power` | 
`public uint64_t reports_count` | 
`public bool with_badge` | 
`public uint64_t badge_id` | 
`public bool validated` | 
`public bool completed` | 
`public bool active` | 
`public eosio::time_point_sec created_at` | 
`public eosio::time_point_sec start_at` | 
`public eosio::time_point_sec expired_at` | 
`public bool is_batch` | 
`public std::vector< uint64_t > batch` | 
`public uint64_t parent_batch_id` | 
`public uint64_t duration` | 
`public bool is_encrypted` | 
`public std::string public_key` | 
`public int64_t total_votes` | 
`public std::vector< eosio::name > voters` | 
`public std::string meta` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bycreator() const` | 
`public inline uint64_t bycurator() const` | 
`public inline uint64_t bydoer() const` | 
`public inline uint64_t bybenefactor() const` | 
`public inline uint64_t bygoal() const` | 
`public inline uint128_t goalandtask() const` | 
`public inline uint64_t byhost() const` | 
`public inline uint64_t bytype() const` | 
`public inline uint64_t bystatus() const` | 
`public inline uint64_t bypriority() const` | 
`public inline uint64_t byhasbadge() const` | 
`public inline uint64_t bybadge() const` | 
`public inline uint128_t crewithtask() const` | 
`public inline uint128_t crewithgoal() const` | 
`public inline uint64_t byvotes() const` | 

## Members

### `public uint64_t task_id` {#structtasks_1ad4b311db95eda69f6aaf6cd511bea146}





### `public uint64_t goal_id` {#structtasks_1aa4501a330653e176762f809b7d84647d}





### `public eosio::name host` {#structtasks_1a9adf310986b0b821249916c5d44e97b9}





### `public eosio::name creator` {#structtasks_1a9c551795c1cdbde451c03705f9f2174b}





### `public eosio::name benefactor` {#structtasks_1a477ff31c38b808eb18496ca7ae6370a6}





### `public eosio::name suggester` {#structtasks_1a9febb6a0d6b990f4773496a203d1a9e2}





### `public std::string permlink` {#structtasks_1a065f2d917e382346cd89e2f77e95472d}





### `public eosio::name type` {#structtasks_1a631f3185010ccc1fc833173335274a03}





### `public eosio::name status` {#structtasks_1acaadd6e375352b79fae8b9e223aee032}





### `public uint64_t priority` {#structtasks_1a5d233f233b15ab37116b60b28d19a189}





### `public bool is_regular` {#structtasks_1adf6c1425eb08beeea49d57f209ce7e85}





### `public std::vector< uint64_t > calendar` {#structtasks_1a830005fe1a99e6c57bf8a1205868fdcf}





### `public bool is_public` {#structtasks_1ab67682b429b14eb0102cbabf586f2542}





### `public eosio::name doer` {#structtasks_1a121059b007ee63fa8a842c150bf60251}





### `public eosio::name role` {#structtasks_1aabceb8533e216dc89e10f9e39acabb1a}





### `public uint64_t level` {#structtasks_1ad2511db83c6b290650c578a77090ed17}





### `public eosio::string title` {#structtasks_1a257a4c8ae9272a001f91c51e62a1953a}





### `public eosio::string data` {#structtasks_1a4e816d6457e29a0623a3f5bcfab21e4c}





### `public eosio::asset requested` {#structtasks_1a370f9d2c835740238fd90bf40d647574}





### `public eosio::asset funded` {#structtasks_1acb852d91b0fc4f5d1bbef0fdf2d0222c}





### `public eosio::asset remain` {#structtasks_1a367e96715a0e358b43c44350d258e5b1}





### `public eosio::asset for_each` {#structtasks_1a92fa22fac621633ab15e080641dc840d}





### `public eosio::name curator` {#structtasks_1adf167668b34739293125f4d95b5c6d4e}





### `public uint64_t gifted_badges` {#structtasks_1aa30ffbcf360d9696a964680178b14c50}





### `public uint64_t gifted_power` {#structtasks_1a36d2f565b26fd32d54b106bf0d1215b9}





### `public uint64_t reports_count` {#structtasks_1a06754b6aa5c97e1be9fd2c4d6c87ec14}





### `public bool with_badge` {#structtasks_1af9fad58b74a1abaf62159f207886d132}





### `public uint64_t badge_id` {#structtasks_1a6f2215e4279786acdf18c2b55c5cd840}





### `public bool validated` {#structtasks_1afe5af40a582c8449c3077190c9cd0a78}





### `public bool completed` {#structtasks_1a83e06aea80c059d8c7fac27ec33d65a7}





### `public bool active` {#structtasks_1a0fc8631b0327820af735ded60237a177}





### `public eosio::time_point_sec created_at` {#structtasks_1ae74d43862baa65c4849a9508dca9fb7e}





### `public eosio::time_point_sec start_at` {#structtasks_1a5a306bc121712422d8daf5e9b6002e63}





### `public eosio::time_point_sec expired_at` {#structtasks_1a3f18f667e301d7d22147dd837b8301ac}





### `public bool is_batch` {#structtasks_1a4b92b0e447cf09f59b21b1bf4e173a9f}





### `public std::vector< uint64_t > batch` {#structtasks_1a20accf3519f1620e33def9da6d73ba74}





### `public uint64_t parent_batch_id` {#structtasks_1a0414fbac42d48d631c985c51d203d8e3}





### `public uint64_t duration` {#structtasks_1a95f88b37716d025c81cfcf9091b7b1e9}





### `public bool is_encrypted` {#structtasks_1a054b40e6f71e3a6e8ee5b060f110980f}





### `public std::string public_key` {#structtasks_1a0fd14a56e1f0f01da069aed7a67fff26}





### `public int64_t total_votes` {#structtasks_1a40d534d86b521f66775ed1b2d391ea38}





### `public std::vector< eosio::name > voters` {#structtasks_1a16b3de55e07255e8252fd85e26688dd3}





### `public std::string meta` {#structtasks_1a5f7ccf26e6ac1b678d8cd6807631a19e}





### `public inline uint64_t primary_key() const` {#structtasks_1aeaa5ac6f325d5b6935371bcf755cfa86}





### `public inline uint64_t bycreator() const` {#structtasks_1ad22f7c4144514d643e926903ef620ddd}





### `public inline uint64_t bycurator() const` {#structtasks_1a7890689f0ecd517df7bb7af23319a8e3}





### `public inline uint64_t bydoer() const` {#structtasks_1a0125dc5f9292e640868c22656dc3d223}





### `public inline uint64_t bybenefactor() const` {#structtasks_1a69b20ca043c8647069901e3502663e09}





### `public inline uint64_t bygoal() const` {#structtasks_1a8e8f40da24535f25d0de8c3f7b99f5a7}





### `public inline uint128_t goalandtask() const` {#structtasks_1a19e89234fbbec30d0358446e8e5c7360}





### `public inline uint64_t byhost() const` {#structtasks_1a40ece529ad989cfbc7c8930a102e37e6}





### `public inline uint64_t bytype() const` {#structtasks_1aa51c0c51b966fa5274a4249246573650}





### `public inline uint64_t bystatus() const` {#structtasks_1a7682fe8e8e3a3579713420f9523c0f72}





### `public inline uint64_t bypriority() const` {#structtasks_1ad184b97d2cff31b025d05e1253044e4d}





### `public inline uint64_t byhasbadge() const` {#structtasks_1a4080c38086acdc1344695a7b2dad9b7f}





### `public inline uint64_t bybadge() const` {#structtasks_1a5499f03e65de7a2ec9c1deb1519707e4}





### `public inline uint128_t crewithtask() const` {#structtasks_1a1f55fdf7e836de26bfe3aef858d7c0c2}





### `public inline uint128_t crewithgoal() const` {#structtasks_1ad7496f1eb5691830783342adcf4a1c2a}





### `public inline uint64_t byvotes() const` {#structtasks_1a646b95d147cb5ae5701c24f729491241}






# struct `usbadges` {#structusbadges}


Структура наградных значков пользователя, полученных от разных хостов Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name host` | 
`public uint64_t badge_id` | 
`public bool netted` | 
`public uint64_t goal_id` | 
`public uint64_t task_id` | 
`public uint64_t count` | 
`public eosio::string caption` | 
`public eosio::string description` | 
`public eosio::string iurl` | 
`public eosio::string pic` | 
`public eosio::string comment` | 
`public uint64_t power` | 
`public eosio::time_point_sec first_recieved_at` | 
`public eosio::time_point_sec last_recieved_at` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t host_key() const` | 
`public inline uint128_t hostandbadge() const` | 
`public inline uint128_t hostandtask() const` | 
`public inline uint128_t hostandgoal() const` | 

## Members

### `public uint64_t id` {#structusbadges_1af54953d38c419d67ad3ed188b9b66705}





### `public eosio::name host` {#structusbadges_1a96cbc9f435656fae107dd40d58a43ad8}





### `public uint64_t badge_id` {#structusbadges_1ab4c257049a4e56ca60beeab27abc86a3}





### `public bool netted` {#structusbadges_1ab2da4322f23274e587cfd0f87a3e4bd9}





### `public uint64_t goal_id` {#structusbadges_1af4f915da566922adcee14bec0d72e854}





### `public uint64_t task_id` {#structusbadges_1a3f7e9a3188d66b4fa9df7eb3efaa20ea}





### `public uint64_t count` {#structusbadges_1ade0ebec9ad5433895f53ebf5455df2de}





### `public eosio::string caption` {#structusbadges_1ae9da4cbbdc410fdf486d507cead73978}





### `public eosio::string description` {#structusbadges_1a3a11a564db4b1572573f68fa7733842e}





### `public eosio::string iurl` {#structusbadges_1ae1ccf8b187bd1286e0e611a5af1efabd}





### `public eosio::string pic` {#structusbadges_1a2b8c2a9315f92a498808043539fa572f}





### `public eosio::string comment` {#structusbadges_1ac570f50eaa4fd0cc045c7fc81e92b291}





### `public uint64_t power` {#structusbadges_1a8b27ed20b270269c5e228dd6b8284bfa}





### `public eosio::time_point_sec first_recieved_at` {#structusbadges_1a1c3a798d3506940e9528deb4d90a54f5}





### `public eosio::time_point_sec last_recieved_at` {#structusbadges_1a8981791289d1c3e1b5e2047770e83de4}





### `public inline uint64_t primary_key() const` {#structusbadges_1aa4163106e97fb9771586b4c8677df30d}





### `public inline uint64_t host_key() const` {#structusbadges_1a042bdd252b5cf0b75f9c4decc0e6fda5}





### `public inline uint128_t hostandbadge() const` {#structusbadges_1a7a6853ea93075e7c8bfc5d511e327b77}





### `public inline uint128_t hostandtask() const` {#structusbadges_1af2c4980c5cab23010df77daba94da40d}





### `public inline uint128_t hostandgoal() const` {#structusbadges_1a4228dc6da303415539b37f62b3f50065}






# struct `userscount` {#structuserscount}


Структура статистики количества зарегистрированных пользователей



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t count` | 
`public eosio::string subject` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structuserscount_1ab26b8b359291bf8d9a27244559fb997f}





### `public uint64_t count` {#structuserscount_1a01c9875465eb8610009cc7030062ebd5}





### `public eosio::string subject` {#structuserscount_1a54f7d250180247c6e8e48c70eb3b9515}





### `public inline uint64_t primary_key() const` {#structuserscount_1ac14da811dfb4568a8b19fea2deb037ae}






# struct `vacs` {#structvacs}


Структура вакансии хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name creator` | 
`public bool approved` | 
`public bool closed` | 
`public eosio::name limit_type` | 
`public eosio::asset income_limit` | 
`public uint64_t proposals` | 
`public uint64_t weight` | 
`public std::string role` | 
`public std::string description` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bycreator() const` | 

## Members

### `public uint64_t id` {#structvacs_1a7ee15c9203c307f828103e1ff269f5d7}





### `public eosio::name creator` {#structvacs_1a92789d98dd7c5145119308688279e71b}





### `public bool approved` {#structvacs_1a8a85a9aa09a4715080494b176ca3d406}





### `public bool closed` {#structvacs_1a11f60e17d33622d6e4309dd8ae08e18f}





### `public eosio::name limit_type` {#structvacs_1afa646f5dc2e17215ba4b0d72b4d67b2c}





### `public eosio::asset income_limit` {#structvacs_1aae267f9df7a2cb666adb997126387dc1}





### `public uint64_t proposals` {#structvacs_1a117f42cdc01ba0d853cbb9f9015e8e99}





### `public uint64_t weight` {#structvacs_1a9cb7a2c336ca54c69178a6cbc488ffb4}





### `public std::string role` {#structvacs_1aeef598457e4bef3ff3c02eb77d5b0177}





### `public std::string description` {#structvacs_1a32302c94a14ab33a5722b03f7ab850a9}





### `public inline uint64_t primary_key() const` {#structvacs_1ae84328b732c9610c831056ffe9afcc4e}





### `public inline uint64_t bycreator() const` {#structvacs_1a37e7e3920a78ac5daf2491bb5991cdca}






# struct `vesting` {#structvesting}


Структура учёта вестинг-балансов пользователей при возврате силы хосту Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name host` | 
`public eosio::name owner` | 
`public eosio::name contract` | 
`public eosio::name type` | 
`public eosio::time_point_sec startat` | 
`public uint64_t duration` | 
`public eosio::asset amount` | 
`public eosio::asset available` | 
`public eosio::asset withdrawed` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structvesting_1a6288cd23eebb2f8bd05aec23647f7d6d}





### `public eosio::name host` {#structvesting_1ab795bee0918bd741aae82e5fb3f053a8}





### `public eosio::name owner` {#structvesting_1a8cac984b9edf45bea6c467398008d3dd}





### `public eosio::name contract` {#structvesting_1a210e4616f4ee4389d3368f2c882294f2}





### `public eosio::name type` {#structvesting_1ac6972755c998eb9887cce0d463dc9bdb}





### `public eosio::time_point_sec startat` {#structvesting_1a75d56f291bccd7edb0fd7097b7b6727f}





### `public uint64_t duration` {#structvesting_1af9d969971c6d91333ae2282a6e05e742}





### `public eosio::asset amount` {#structvesting_1a1de75a68c605d780b132c09749d957b0}





### `public eosio::asset available` {#structvesting_1acd1b02df1f7b77e08e64dee70c93dea5}





### `public eosio::asset withdrawed` {#structvesting_1ab60d72a07a57b50a714c8638527fe056}





### `public inline uint64_t primary_key() const` {#structvesting_1a35ede92a5445d2ab793b0bbbc18c5acb}






# struct `votes` {#structvotes}


Структура голосов за цели хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t goal_id` | 
`public eosio::name host` | 
`public int64_t power` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byhost() const` | 
`public inline uint128_t idwithhost() const` | 

## Members

### `public uint64_t id` {#structvotes_1aa3e80b46ca3e30af1ecd42323adeb2d3}





### `public uint64_t goal_id` {#structvotes_1a1b7b64166bd63d0edb024401ddba2615}





### `public eosio::name host` {#structvotes_1a3e9666701df6922b45727fce90a9cfae}





### `public int64_t power` {#structvotes_1a61a8b9a9f031e027b6ab7da38bd7c00f}





### `public inline uint64_t primary_key() const` {#structvotes_1a7e45203e8e17bd3321915a38edfc936e}





### `public inline uint64_t byhost() const` {#structvotes_1a9a9364ba8ca4789b5b91af3f0d0a1803}





### `public inline uint128_t idwithhost() const` {#structvotes_1ac66dd6322d7aead5f453001d9410f238}






# struct `vproposal` {#structvproposal}


Структура заявки на вакансию хоста Двойной Спирали.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t vac_id` | 
`public eosio::name creator` | 
`public eosio::name limit_type` | 
`public eosio::name income_limit` | 
`public uint64_t weight` | 
`public bool closed` | 
`public std::string why_me` | 
`public std::string contacts` | 
`public int64_t votes` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byusername() const` | 
`public inline uint64_t byvacid() const` | 
`public inline uint64_t byvotes() const` | 

## Members

### `public uint64_t id` {#structvproposal_1a2256401ab4ef42873de7d421ebdda8d2}





### `public uint64_t vac_id` {#structvproposal_1ab50453667543dd87a6322b23465a1c82}





### `public eosio::name creator` {#structvproposal_1a33b76d80622c5b1240b4648dc8853736}





### `public eosio::name limit_type` {#structvproposal_1a8842ccf178234200fafd9fe1a05b5d77}





### `public eosio::name income_limit` {#structvproposal_1a556c2c05b8a28dd3b25000a9ad64ecea}





### `public uint64_t weight` {#structvproposal_1a001c700627f80f5437cf4b24aa00d49f}





### `public bool closed` {#structvproposal_1a26964a820d84cff45449069c1d5d0fd0}





### `public std::string why_me` {#structvproposal_1a8d187e3700848b2b6d78e6c2e3a01b2c}





### `public std::string contacts` {#structvproposal_1a60b006e66f1ecb7ab8dae173e5d5970b}





### `public int64_t votes` {#structvproposal_1af50598bc4dd264f992d89c381229872b}





### `public inline uint64_t primary_key() const` {#structvproposal_1a1808a81bea55e9cb4ae86c3bbaf839c5}





### `public inline uint64_t byusername() const` {#structvproposal_1a64731d926c4cf39b9f52516588653064}





### `public inline uint64_t byvacid() const` {#structvproposal_1a31c5443a100d694e82cfa38875cc9e21}





### `public inline uint64_t byvotes() const` {#structvproposal_1a1a2eb6f0681aa2369a5c944394a3369c}






