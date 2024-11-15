CREATE TABLE IF NOT EXISTS humidity(
    controller_id INTEGER NOT NULL,
    timestamp TIMESTAMP NOT NULL,
    value DECIMAL(3, 2) NOT NULL,
    PRIMARY KEY (controller_id, timestamp)
);

CREATE TABLE IF NOT EXISTS temperature(
    controller_id INTEGER NOT NULL,
    timestamp TIMESTAMP NOT NULL,
    value DECIMAL(3, 2) NOT NULL,
    PRIMARY KEY (controller_id, timestamp)
);

CREATE TABLE IF NOT EXISTS pressure(
    controller_id INTEGER NOT NULL,
    timestamp TIMESTAMP NOT NULL,
    value DECIMAL(3, 2) NOT NULL,
    PRIMARY KEY (controller_id, timestamp)
);