--enabled for ON DELETE CASCADE
PRAGMA foreign_keys = ON;

--All paths could be in one table, but if paths start with something in common
--it's a litte more efficient to direct them to their root.
--Root paths may also have meta data
--INTEGER PRIMARY KEY is the same as ROWID
--AUTOINCREMENT is avoided though since that would prevent reuse of previously deleted ids
--The check for the depth is set to 2048 since on linux the max path size
--is generally accepted to be 4098. Technically a limit doesn't need to exist,
--but this is what PATH_MAX generally is. This size includes the entire path
--string which includes '/', so if each directory were to be a single character
--it leaves 2048 sub dirs available.

CREATE TABLE IF NOT EXISTS Roots(
    root_id INTEGER PRIMARY KEY,
    root_type INTEGER CHECK (root_type IN (4, 3, 2, 1)) NOT NULL,
    root_name VARCHAR(4096) NOT NULL,
    root_length INTEGER CHECK (root_length > 0 AND root_length < 4096) NOT NULL,
    root_depth INTEGER CHECK ((root_depth <= 2048 AND root_depth >= 0) OR root_depth == -5) NOT NULL,
    UNIQUE(root_name, root_id)
);

CREATE TABLE IF NOT EXISTS Paths(
    path_id INTEGER PRIMARY KEY,
    root_id INTEGER NOT NULL,
    path_name VARCHAR(4096) NOT NULL,
    path_length INTEGER CHECK (path_length > 0) NOT NULL,
    FOREIGN KEY (root_id) REFERENCES Roots (root_id) ON DELETE CASCADE
);
