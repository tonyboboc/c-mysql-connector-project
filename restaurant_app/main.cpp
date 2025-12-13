#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <mysql/jdbc.h>
void show_table(const std::string & table,sql::Connection *con){

}
int main()
{
    const std::string url    = "tcp://127.0.0.1:3306";
    const std::string user   = "mysql";
    const std::string pass   = "mysql1234";
    const std::string schema = "mydb";
    const std::string cook_tb="cook_to_do";
    const std::string barman_tb="barman_to_do";
    try
    {

        sql::Driver* driver = get_driver_instance();

    
        std::unique_ptr<sql::Connection> con(
            driver->connect(url, user, pass)
        );
        con->setSchema(schema);
        std::map<int,std::string> choice={{1,cook_tb},{0,barman_tb}};
        int x;
        std::cout<<"what are you , a cook or a barman ? (1=cook) (0=barman) ";
        std::cin>>x;
        while(x<0||x>1){
            std::cout<<"you didn't enter a right key , press again ";
            std::cin>>x;
        }

       
    }
    catch (const sql::SQLException& e)
    {
        std::cerr << "MySQL error: " << e.what() << '\n';
        std::cerr << "Error code: " << e.getErrorCode() << '\n';
        std::cerr << "SQLState: " << e.getSQLState() << '\n';
    }

    return 0;
}