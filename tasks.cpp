namespace eosio{

struct tsks
{

	void settask_action (const settask &op){
		
		require_auth(op.host);
		account_index accounts(_self, _self);
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");


		auto root_symbol = acc->get_root_symbol();
		eosio_assert(op.requested.symbol == root_symbol, "Wrong token for current host" );
		eosio_assert(op.for_each.symbol == root_symbol, "Wrong token for current host" );

		goals_index goals(_self, op.host);
		auto goal = goals.find(op.goal_id);
		eosio_assert(goal != goals.end(), "Goal is not found");

		tasks_index tasks(_self, op.host);

		tasks.emplace(op.host, [&](auto &t){
			t.task_id = tasks.available_primary_key();
			t.goal_id = op.goal_id;
			t.title = op.title;
			t.data = op.data;
			t.requested = op.requested;
			t.for_each = op.for_each;
			t.funded = asset(0, root_symbol);
			t.remain = asset(0, root_symbol);
			t.is_public  = op.is_public;
			t.with_badge = op.with_badge;
			t.badge_type = op.badge_type;
			t.expired_at = eosio::time_point_sec (now() + op.expiration);
		});

	}


	void fundtask_action(const fundtask &op){
		require_auth(op.host);

		account_index accounts(_self, _self);
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");
		
		tasks_index tasks(_self, op.host);
		auto task = tasks.find(op.task_id);
		eosio_assert(task != tasks.end(), "Task is not exist");
		goals_index goals(_self, op.host);
		
		auto goal = goals.find(task->goal_id);
		
		eosio_assert(goal != goals.end(), "Goal is not found");

		eosio_assert(goal->available >= op.amount, "Not enough tokens for fund in the goal object");

		eosio_assert(task->funded + op.amount <= task->requested, "Fund amounts is more then requested");

		goals.modify(goal, op.host, [&](auto &g){
			g.available = goal->available - op.amount;
			g.withdrawed = goal->withdrawed + op.amount;
		});

		tasks.modify(task, op.host, [&](auto &t){
			t.funded = task->funded + op.amount;
			t.remain = task->remain + op.amount;
		});

	}

	void tactivate_action (const tactivate &op){
		require_auth(op.host);
		account_index accounts(_self, _self);
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_self, op.host);

		auto task = tasks.find(op.task_id);

		eosio_assert(task != tasks.end(), "Task is not found");

		eosio_assert(task->active == false, "Task is already active");

		tasks.modify(task, op.host, [&](auto &t){
			t.active = true;
		});

	}
	
	void tdeactivate_action (const tdeactivate &op){
		require_auth(op.host);
		account_index accounts(_self, _self);
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_self, op.host);

		auto task = tasks.find(op.task_id);

		eosio_assert(task != tasks.end(), "Task is not found");

		eosio_assert(task->active == true, "Task is already deactivated");

		tasks.modify(task, op.host, [&](auto &t){
			t.active = false;
		});

	}

	void setreport_action (const setreport &op){
		// account_name host;
		// account_name username;
		// uint64_t task_id;
		// eosio::string data;

		require_auth(op.username);
		account_index accounts(_self, _self);
		user_index users(_self, _self);
		auto user = users.find(op.username);
		eosio_assert(user != users.end(), "User is not registered on the core");
		
		auto acc = accounts.find(op.host);
		auto root_symbol = acc->get_root_symbol();
		
		// eosio_assert(root_symbol == op.requested.symbol, "Wrong token for current host");
		eosio_assert(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_self, op.host);
		auto task = tasks.find(op.task_id);
		eosio_assert(task != tasks.end(), "Task is not found");
		eosio_assert(task-> active == true, "Task is not active");
		
		
		//check by complex key USERNAME & ID
		//IF not exist - do

		reports_index reports(_self, op.host);
		
		auto users_with_id = reports.template get_index<N(user_with_task)>();

		auto report_ids = combine_ids(op.username, op.task_id);
		auto user_report = users_with_id.find(report_ids);

		eosio_assert(user_report == users_with_id.end(), "Report for this task already exist");

		reports.emplace(op.username, [&](auto &r){
			r.report_id = reports.available_primary_key();
			r.task_id = op.task_id;
			r.goal_id = task->goal_id;
			r.username = op.username;
			r.data = op.data;
			r.need_check = true;
			r.requested = task->for_each;
			r.curator = op.host;
		});

	}

	void editreport_action (const editreport &op){
		// require_auth(op.voter);
		require_auth(op.username);
		account_index accounts(_self, _self);
		
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");

		reports_index reports(_self, op.host);
		auto report = reports.find(op.report_id);

		eosio_assert(report->need_check == false, "Cannot modify report until check");

		reports.modify(report, op.username, [&](auto &r){
			r.need_check = true;
			r.data = op.data;
		});
	}

	void approver_action (const approver &op){
		// require_auth(op.voter);
		require_auth(op.host);
		
		account_index accounts(_self, _self);
		
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");

		reports_index reports(_self, op.host);
		auto report = reports.find(op.report_id);
		
		tasks_index tasks(_self, op.host);
		auto task = tasks.find(report->task_id);
	
		
		eosio_assert(report->requested <= task->remain, "Not enough funds for pay to this task");

		eosio_assert(report->approved == false, "Task is already approved");
		
		reports.modify(report, op.host, [&](auto &r){
			r.need_check = false;
			r.comment = op.comment;
			r.approved = true;
		});


		if (report->requested.amount > 0){
			action(
	                permission_level{ _self, N(active) },
	                acc->root_token_contract, N(transfer),
	                std::make_tuple( _self, report->username, report->requested, op.comment) 
	        ).send();
		};

		tasks.modify(task, op.host, [&](auto &t){
			t.remain = task->remain - report->requested;
		});

		giftbadge gift;
		gift.host = op.host;
		gift.to = report->username;
		gift.badge_type = task->badge_type;
		gift.comment = std::string("Completed task");


		badge_struct().giftbadge_action(gift);

	
	}

	void disapprover_action (const disapprover &op){
		require_auth(op.host);
		account_index accounts(_self, _self);
		
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");

		reports_index reports(_self, op.host);
		auto report = reports.find(op.report_id);

		eosio_assert(report->approved == false, "Report is already approved and cannot be disapproved now.");

		reports.modify(report, op.host, [&](auto &r){
			r.need_check = false;
			r.comment = op.comment;
		});

	}
		

};
}