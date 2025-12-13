#include <iostream>
#include <memory>

#include <mysql/jdbc.h>

int main()
{
    const std::string url    = "tcp://127.0.0.1:3306";
    const std::string user   = "mysql";
    const std::string pass   = "mysql1234";
    const std::string schema = "mydb";

    try
    {
        sql::Driver* driver = get_driver_instance();

    
        std::unique_ptr<sql::Connection> con(
            driver->connect(url, user, pass)
        );

    
        con->setSchema(schema);


       
    }
    catch (const sql::SQLException& e)
    {
        std::cerr << "MySQL error: " << e.what() << '\n';
        std::cerr << "Error code: " << e.getErrorCode() << '\n';
        std::cerr << "SQLState: " << e.getSQLState() << '\n';
    }

    return 0;
}