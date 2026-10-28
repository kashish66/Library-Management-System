#define CPPHTTPLIB_IMPLEMENTATION
#include "httplib.h"
#include "json.hpp"

#include <fstream>
#include <iostream>
#include <string>

using json = nlohmann::json;
using namespace std;

const string DATA_FILE = "books.json";

json readBooks() {
    ifstream file(DATA_FILE);
    if (!file.is_open()) return json::array();
    json data;
    try {
        file >> data;
        if (!data.is_array()) return json::array();
        return data;
    } catch (...) {
        return json::array();
    }
}

void writeBooks(const json& books) {
    ofstream file(DATA_FILE);
    file << books.dump(2);
}

int main() {
    httplib::Server server;

    server.Get("/api/books", [](const httplib::Request& req, httplib::Response& res) {
        json books = readBooks();
        if (req.has_param("q")) {
            string q = req.get_param_value("q");
            json filtered = json::array();
            for (const auto& b : books) {
                string title = b.value("title", "");
                string author = b.value("author", "");
                string lowerQ = q;
                for (char& c : lowerQ) c = (char)tolower((unsigned char)c);
                for (char& c : title) c = (char)tolower((unsigned char)c);
                for (char& c : author) c = (char)tolower((unsigned char)c);
                if (title.find(lowerQ) != string::npos || author.find(lowerQ) != string::npos)
                    filtered.push_back(b);
            }
            books = filtered;
        }
        res.set_content(books.dump(2), "application/json");
    });

    server.Post("/api/books", [](const httplib::Request& req, httplib::Response& res) {
        json body;
        try { body = json::parse(req.body); }
        catch (...) { res.status = 400; res.set_content(R"({"error":"Invalid JSON"})", "application/json"); return; }

        if (!body.contains("id") || !body.contains("title") || !body.contains("author")) {
            res.status = 400; res.set_content(R"({"error":"ID, title and author are required"})", "application/json"); return;
        }

        json books = readBooks();
        int id = body["id"].get<int>();
        for (const auto& b : books) {
            if (b.value("id", -1) == id) {
                res.status = 409; res.set_content(R"({"error":"Book ID already exists"})", "application/json"); return;
            }
        }

        books.push_back({{"id", id}, {"title", body["title"]}, {"author", body["author"]}, {"issued", false}});
        writeBooks(books);
        res.set_content(R"({"message":"Book added"})", "application/json");
    });

    server.Post(R"(/api/books/(\d+)/issue)", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        json books = readBooks();
        for (auto& b : books) {
            if (b.value("id", -1) == id) {
                if (b.value("issued", false)) { res.status = 409; res.set_content(R"({"error":"Book is already issued"})", "application/json"); return; }
                b["issued"] = true; writeBooks(books);
                res.set_content(R"({"message":"Book issued"})", "application/json"); return;
            }
        }
        res.status = 404; res.set_content(R"({"error":"Book not found"})", "application/json");
    });

    server.Post(R"(/api/books/(\d+)/return)", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        json books = readBooks();
        for (auto& b : books) {
            if (b.value("id", -1) == id) {
                if (!b.value("issued", false)) { res.status = 409; res.set_content(R"({"error":"Book is not issued"})", "application/json"); return; }
                b["issued"] = false; writeBooks(books);
                res.set_content(R"({"message":"Book returned"})", "application/json"); return;
            }
        }
        res.status = 404; res.set_content(R"({"error":"Book not found"})", "application/json");
    });

    server.Delete(R"(/api/books/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        json books = readBooks();
        for (size_t i = 0; i < books.size(); ++i) {
            if (books[i].value("id", -1) == id) {
                if (books[i].value("issued", false)) { res.status = 409; res.set_content(R"({"error":"Return this book before deleting it"})", "application/json"); return; }
                books.erase(i); writeBooks(books);
                res.set_content(R"({"message":"Book deleted"})", "application/json"); return;
            }
        }
        res.status = 404; res.set_content(R"({"error":"Book not found"})", "application/json");
    });

    server.set_mount_point("/", "./public");
    cout << "\nLibrary Management System is running!\n";
    cout << "Open Chrome at: http://localhost:8080\n";
    cout << "Keep this terminal open while using the website.\n\n";
    if (!server.listen("127.0.0.1", 8080)) {
        cerr << "Could not start server. Is port 8080 already in use?\n";
        return 1;
    }
    return 0;
}
