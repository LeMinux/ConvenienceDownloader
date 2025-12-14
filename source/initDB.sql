--All paths could be in one table, but if paths start with something in common
--it's a litte more efficient to direct them to their root.
--Root paths may also have meta data
CREATE TABLE IF NOT EXISTS Roots(
    root_id INTEGER PRIMARY KEY,
    config_type INTEGER CHECK (config_type IN (3, 2, 1)) NOT NULL,
    root_name VARCHAR(4096) NOT NULL,
    root_length INTEGER CHECK (root_length > 0 AND root_length < 4096),
    depth INTEGER CHECK (depth <= 4096 AND depth >= -1) NOT NULL
);

CREATE TABLE IF NOT EXISTS Paths(
    path_index INTEGER PRIMARY KEY,
    root_id INTEGER NOT NULL,
    path VARCHAR(4096),
    path_length INTEGER,
    FOREIGN KEY (root_id) REFERENCES Roots (root_id)
);

CREATE TABLE IF NOT EXISTS Blacklist(
    black_index INTEGER PRIMARY KEY,
    black_path VARCHAR(4096) NOT NULL,
    black_length INTEGER CHECK (black_length > 0 AND black_length < 4096)
);

--Consider maybe making this inside the code
--as sqlite doesn't have direct support for variables
/*
CREATE TRIGGER IF NOT EXISTS checkTotalLength BEFORE INSERT ON Paths
FOR EACH ROW
BEGIN
    CREATE TEMP TABLE IF NOT EXISTS Vars (name TEXT PRIMARY KEY, value INTEGER);
    INSERT OR REPLACE INTO Vars VALUES("length_of_root", 0);

    DECLARE total_length;
    DECLARE length_of_root;

    SELECT root_length INTO length_of_root FROM Roots WHERE root_id = NEW.associated_root;

    SET total_length = length_of_root + NEW.length;
    IF total_length > 4096 THEN
        RAISE(ABORT, "Path inserting has a length greater than 4096 when accounting for the root");
    END IF;
END;
*/
