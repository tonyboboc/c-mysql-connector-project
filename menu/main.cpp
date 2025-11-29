
#include <mysql/jdbc.h>   // classic / JDBC-like API
#include <iostream>
#include <memory>

int main() {
    try {
        // Connection info
        const std::string url  = "tcp://127.0.0.1:3306"; // classic protocol port
        const std::string user = "mysql";
        const std::string pass = "mysql1234";
        const std::string schema = "mydb";

        // Get driver instance
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_driver_instance();

        // Create connection
        std::unique_ptr<sql::Connection> conn(
            driver->connect(url, user, pass)
        );

        // Select schema (database)
        conn->setSchema(schema);

        // Create table (similar idea to your collection)
        {
            std::unique_ptr<sql::Statement> stmt(conn->createStatement());
            stmt->execute(
                "CREATE TABLE IF NOT EXISTS test_table ("
                "  id   INT AUTO_INCREMENT PRIMARY KEY,"
                "  name VARCHAR(64),"
                "  age  INT"
                ")"
            );
        }

        // Insert a row (like your document add)
        {
            std::unique_ptr<sql::PreparedStatement> prep(
                conn->prepareStatement(
                    "INSERT INTO test_table(name, age) VALUES(?, ?)"
                )
            );
            prep->setString(1, "Alice");
            prep->setInt(2, 30);
            prep->execute();
        }

        // Query rows: age > 20 (like your find)
        {
            std::unique_ptr<sql::PreparedStatement> prep(
                conn->prepareStatement(
                    "SELECT id, name, age FROM test_table WHERE age > ?"
                )
            );
            prep->setInt(1, 20);
            std::unique_ptr<sql::ResultSet> res(prep->executeQuery());

            while (res->next()) {
                int id        = res->getInt("id");
                std::string n = res->getString("name");
                int age       = res->getInt("age");

                std::cout << "id=" << id
                          << ", name=" << n
                          << ", age=" << age << '\n';
            }
        }

    } catch (const sql::SQLException &e) {
        std::cerr << "MySQL error: " << e.what()
                  << " (code: " << e.getErrorCode()
                  << ", SQLState: " << e.getSQLState() << ")\n";
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "STD exception: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception!\n";
        return 1;
    }

    return 0;
}
