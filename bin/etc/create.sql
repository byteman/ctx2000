CREATE TABLE tcparam (No INTEGER NOT NULL PRIMARY KEY,Rate TEXT NOT NULL, Length TEXT NOT NULL,Distance TEXT NOT NULL,Weight TEXT NOT NULL, Type TEXT NOT NULL);
CREATE INDEX "rate" on tcparam (Rate ASC);
CREATE INDEX "type" on tcparam (Type ASC);
CREATE INDEX "length" on tcparam (Length ASC);
