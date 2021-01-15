#include "../headers/includes.h"
using json = nlohmann::json;
using namespace std::literals::chrono_literals;
Instagram::Instagram() {
	UUID = GetUUID();
	RequestsSession.SetUserAgent(cpr::UserAgent("Instagram 22.0.0.15.68 Android (24/5.0; 515dpi; 1440x2416; huawei/google; Nexus 6P; angler; angler; en_US)"));
	RequestsSession.SetHeader(cpr::Header{
		{"Accept-Language", "en;q=0.9"},
		{"ContentType", "application/x-www-form-urlencoded; charset=UTF-8"}
		});
}

std::string Instagram::GetUUID() {
	static std::random_device rDevice;
	static std::mt19937 range(rDevice());
	std::uniform_int_distribution<int> dist(0, 15);
	const char* chars = "0123456789abcdefg";
	const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };
	std::string uuid;
	for (int i = 0; i < 16; i++) {
		if (dash[i]) uuid += "-";
		uuid += chars[dist(range)];
		uuid += chars[dist(range)];
	}
	return uuid;
}

cpr::Response Instagram::SessoinRequest(const std::string& url, const cpr::Payload& post) {
	RequestsSession.SetUrl(cpr::Url{ url });
	if (post.content.length() == 0) {
		return RequestsSession.Get();
	}
	else {
		RequestsSession.SetPayload(post);
		return RequestsSession.Post();
	}
}

int Instagram::GetFollowersCount() {
	cpr::Response response = SessoinRequest("https://i.instagram.com/api/v1/users/" + AccountID + "/info/");
	if (response.status_code == 200) {
		json j = json::parse(response.text);
		return (int)j["user"]["follower_count"];
	}
	return 0;
}

std::vector<std::string> Instagram::GetTotalFollowers() {
	std::string next_max_id = "";
	std::vector<std::string> Followers;
	Followers.reserve(GetFollowersCount()); // reserve the array to fit the followers count 
	// now every request should give us 193 username
	while (true) {
		cpr::Response response = SessoinRequest("https://i.instagram.com/api/v1/friendships/" + AccountID + "/followers/?max_id=" + next_max_id);
		// Check if Request is successful
		if (response.status_code == 200) {
			json j = json::parse(response.text);
			for (auto jo : j["users"]) {
				Followers.emplace_back(jo["username"]);
			}
			if (j["big_list"] == false) // Checking if this's the last list of followers
				break;
			next_max_id = j["next_max_id"];
		}
		else {
			std::this_thread::sleep_for(2min); // RateLimit 
		}
		std::this_thread::sleep_for(100ms);
	}
	return Followers;
}
void Instagram::GetUpdate() {
	std::vector<std::string> FollowersArray;
	FollowersArray.reserve(GetFollowersCount());
	FollowersArray = GetTotalFollowers();
	std::this_thread::sleep_for(2min);
	std::vector<std::string> Followers = GetTotalFollowers();
	// First We will check for unfollowers then the new followers
	for (std::string element : FollowersArray) {
		if (!(std::find(Followers.begin(), Followers.end(), element) != Followers.end()))
			std::cout << "UnFollow: @" << element << std::endl; // UnFollow 
	}
	// Check for new Followers
	for (std::string element : Followers) {
		if (!(std::find(FollowersArray.begin(), FollowersArray.end(), element) != FollowersArray.end()))
			std::cout << "Follow: @" << element << std::endl; // Follow 
	}
}

bool Instagram::Login(const std::string& username, const std::string& password) {
	cpr::Response response = SessoinRequest("https://i.instagram.com/api/v1/accounts/login/", cpr::Payload{
		{"username", username},
		{"password", password},
		{"_uuid", UUID},
		{"device_id", UUID},
		{"login_attempt_count", "0"},
		{"_csrftoken", "missing"},
		{"from_reg", "false"}
		});
	if (response.text.find("logged_in_user") != std::string::npos) {
		// Storage The Cookies And Set it to the Session
		std::string AccountCookies;
		for (std::pair<const std::string, std::string> cookie : response.cookies) {
			AccountCookies.append(cookie.first + "=" + cookie.second + "; ");
			if (cookie.first.compare("ds_user_id") == 0)
				AccountID = cookie.second;
		}
		RequestsSession.SetHeader({ { "Cookie", AccountCookies } });
		return 1;
	}
	return 0;
}