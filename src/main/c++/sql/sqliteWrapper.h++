#ifndef TYRANT_CACHE_SQL_SQLITEWRAPPER_HPP
    #define TYRANT_CACHE_SQL_SQLITEWRAPPER_HPP

    // We use strings, in fact most fields in the database are stored as strings
    #include <string>
    // A record is basically a map
    #include <map>
    // Many results are stored in a vector
    #include <vector>
    #include <set>
    #include <sqlite3.h>
    

    namespace TyrantCache {
        namespace SQL {

            /**
             * A generic way to store one SQL record.
             * Basically this is just an associative map and some fancy getters.
             */
            class SQLResult {
                private:
                    std::map<std::string, std::string> data;

                public:
                    void put(std::string const & name, std::string const & data);
                    unsigned long getULong(std::string const & column) const;
                    std::string getString(std::string const & column) const;
            };

            /**
             * A vector of records.
             */
            typedef std::vector<SQLResult> SQLResults;

            /**
             * An sql statement is just a string for now.
             * However a prepared statement is more complex.
             */
            typedef std::string Statement;


            /**
             * A prepared statement provides a C++ view on the sqlite3 functions for prepared statements.
             */
            class PreparedStatement {
                private:
                    sqlite3_stmt * statement;
                    
                public:
                    PreparedStatement(sqlite3_stmt *);
                    ~PreparedStatement();

                    void bindText(unsigned int index, std::string text);
                    void bindInt(unsigned int index, int value);
                    void bindNull(unsigned int index);
                    void execute();
                    SQLResults query();
            };

            /**
             * This wraps the connection to a sqlite3 database.
             */
            class SQLiteWrapper {
                private:
                    /**
                     * The database
                     */
                    sqlite3 * database;

                    /**
                     * All generated statements
                     */
                    std::set<PreparedStatement *> statements;

                public:

                    /**
                     * Create or open a sqlite database from a given file.
                     * @param fileName path to the file
                     */
                    SQLiteWrapper(std::string const & fileName);
                    ~SQLiteWrapper();

                    /**
                     * Prepares a statement.
                     * @param statement sql description, may contain placeholders
                     * @return the prepared statement
                     */
                    PreparedStatement * prepareStatement(Statement const & statement);

                    /**
                     * Executes a (unprepared) statement.
                     * (To execute a prepared statement see PreparedStatement's methods)
                     */
                    void execute(Statement const &);
                    
            };
            
        }
    }
        
#endif
