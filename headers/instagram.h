#pragma once
#include "includes.h"

class Instagram {
private:
	// Variables :
	// Instagram username id , use to get Account data and followers
	std::string AccountID;
	// Session for cpr client
	cpr::Session RequestsSession;
	// UUID key for auth
	std::string UUID;
	/*==================*/
	// Functions :
	// Generate UUID key
	std::string GetUUID();
	// Send http requests to instagram
	cpr::Response SessoinRequest(const std::string& url, const cpr::Payload& post = cpr::Payload{});
	// Get number for Followers count to reserve the arrays
	int GetFollowersCount();
	// Get all followers usernames
	std::vector<std::string> GetTotalFollowers();
public:
	Instagram();
	// Login into the account
	bool Login(const std::string& username, const std::string& password);
	// Check Followers by username depending on a new Followers-list and another that was checked a few minutes ago
	void GetUpdate();
};