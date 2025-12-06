
#include <mysql/jdbc.h>  
#include <iostream>
#include <memory>
#include <string>
void show_menu(const std::string & menu,sql::Connection *con){
        sql::Statement *stmt;
        sql::ResultSet *res;
        std::string sql_statement="Select * from "+menu+";";
        stmt=con->createStatement();
        res=stmt->executeQuery(sql_statement);
        while(res->next()){
            std::cout<<"id= "<<res->getInt(1)<<" ";
            std::cout<<" | name of item : "<<res->getString(2);
            std::cout<<" | price : "<<res->getDouble(3);
            std::cout<<std::endl;
        }
        delete res;
        delete stmt;
}
void order(const std::string menu){

}
int main() {
    try {

        const std::string url  = "tcp://127.0.0.1:3306"; 
        const std::string user = "mysql";
        const std::string pass = "mysql1234";
        const std::string schema = "mydb";
        const std::string bar_menu="menu_bar";
        const std::string food_menu="menu_food";
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_driver_instance();

        
        std::unique_ptr<sql::Connection> con(
            driver->connect(url, user, pass)
        );
        con->setSchema(schema);
        show_menu(bar_menu,con.get());
        std::cout<<"now showing food menu\n";
        show_menu(food_menu,con.get());
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
