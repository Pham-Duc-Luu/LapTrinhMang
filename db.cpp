#include <iostream>
#include <sqlite3.h>

void executeSQL(sqlite3* db, const char* sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cout << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Executed successfully.\n";
    }
}

int main() {
    sqlite3* db;

    // 1. Mở hoặc tạo SQLite file
    int rc = sqlite3_open("cinema.db", &db);
    if (rc != SQLITE_OK) {
        std::cout << "Không mở được DB: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    // 2. SQL tạo bảng
    const char* sql =
        // Users
        "CREATE TABLE IF NOT EXISTS users ("
        "  userId TEXT PRIMARY KEY,"
        "  phoneNumber TEXT UNIQUE,"
        "  password TEXT,"
        "  fullName TEXT"
        ");"

        // Movies
        "CREATE TABLE IF NOT EXISTS movies ("
        "  movieId TEXT PRIMARY KEY,"
        "  title TEXT,"
        "  description TEXT,"
        "  duration INTEGER,"
        "  posterUrl TEXT"
        ");"

        // Movie Actors
        "CREATE TABLE IF NOT EXISTS movie_actors ("
        "  movieId TEXT,"
        "  actorName TEXT,"
        "  PRIMARY KEY (movieId, actorName),"
        "  FOREIGN KEY (movieId) REFERENCES movies(movieId)"
        ");"

        // Showtimes
        "CREATE TABLE IF NOT EXISTS showtimes ("
        "  showtimeId TEXT PRIMARY KEY,"
        "  movieId TEXT,"
        "  dateTime TEXT,"
        "  room TEXT,"
        "  FOREIGN KEY (movieId) REFERENCES movies(movieId)"
        ");"

        // Seats
        "CREATE TABLE IF NOT EXISTS seats ("
        "  seatId TEXT PRIMARY KEY"
        ");"

        // Showtime Seats
        "CREATE TABLE IF NOT EXISTS showtime_seats ("
        "  showtimeId TEXT,"
        "  seatId TEXT,"
        "  isReserved INTEGER,"
        "  PRIMARY KEY (showtimeId, seatId),"
        "  FOREIGN KEY (showtimeId) REFERENCES showtimes(showtimeId),"
        "  FOREIGN KEY (seatId) REFERENCES seats(seatId)"
        ");"

        // Reservations
        "CREATE TABLE IF NOT EXISTS reservations ("
        "  reservationId TEXT PRIMARY KEY,"
        "  userId TEXT,"
        "  showtimeId TEXT,"
        "  expiresAt TEXT,"
        "  FOREIGN KEY (userId) REFERENCES users(userId),"
        "  FOREIGN KEY (showtimeId) REFERENCES showtimes(showtimeId)"
        ");"

        // Reservation Seats
        "CREATE TABLE IF NOT EXISTS reservation_seats ("
        "  reservationId TEXT,"
        "  seatId TEXT,"
        "  PRIMARY KEY (reservationId, seatId),"
        "  FOREIGN KEY (reservationId) REFERENCES reservations(reservationId),"
        "  FOREIGN KEY (seatId) REFERENCES seats(seatId)"
        ");";

    // 3. Thực thi SQL
    executeSQL(db, sql);

    // 4. Đóng DB
    sqlite3_close(db);

    std::cout << "Database created successfully: cinema.db\n";

    return 0;
}
