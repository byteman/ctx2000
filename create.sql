CREATE TABLE sqlite_sequence(name,seq);
CREATE TABLE tblprotarea (
    "height" TEXT NOT NULL,
    "x0" TEXT NOT NULL,
    "y0" TEXT NOT NULL,
    "x1" TEXT NOT NULL,
    "y1" TEXT NOT NULL,
    "x2" TEXT NOT NULL,
    "y2" TEXT,
    "x3" TEXT,
    "y3" TEXT,
    "x4" TEXT,
    "y4" TEXT,
    "x5" TEXT,
    "y5" TEXT,
    "index" TEXT
);
CREATE TABLE "tbldividarea" (
    "index" INTEGER PRIMARY KEY NOT NULL,
    "x0" TEXT,
    "y0" TEXT,
    "x1" TEXT,
    "y1" TEXT,
    "x2" TEXT,
    "y2" TEXT,
    "x3" TEXT,
    "y3" TEXT,
    "x4" TEXT,
    "y4" TEXT,
    "x5" TEXT,
    "y5" TEXT
);
CREATE TABLE tblcalc (
    "angle_zero_ad" TEXT NOT NULL DEFAULT (0)
, "angle_calc_k" TEXT  NOT NULL  DEFAULT (1), "car_zero_dist_ad" TEXT  NOT NULL  DEFAULT (0), "car_calc_k" TEXT  NOT NULL  DEFAULT (1));
CREATE TABLE "tbldev" (
    "name" TEXT NOT NULL
);
CREATE TABLE tblworksite (
    "x0" TEXT NOT NULL DEFAULT (0),
    "y0" TEXT NOT NULL DEFAULT (0),
    "x1" TEXT NOT NULL DEFAULT (0),
    "y1" TEXT NOT NULL DEFAULT (0),
    "x2" TEXT NOT NULL DEFAULT (0),
    "y2" TEXT NOT NULL DEFAULT (0),
    "x3" TEXT NOT NULL DEFAULT (0),
    "y3" TEXT NOT NULL DEFAULT (0),
    "y_angle" TEXT NOT NULL DEFAULT (0),
    "alert_dist" TEXT NOT NULL DEFAULT (0),
    "slow_dist" TEXT NOT NULL DEFAULT (0),
    "brake_dist" TEXT NOT NULL DEFAULT (0),
    "car_high_spd_dist" TEXT NOT NULL DEFAULT (0),
    "car_low_spd_dist" TEXT NOT NULL DEFAULT (0),
    "calc_x0" TEXT NOT NULL DEFAULT (0),
    "calc_y0" TEXT NOT NULL DEFAULT (0),
    "calc_x1" TEXT NOT NULL DEFAULT (0),
    "calc_y1" TEXT NOT NULL DEFAULT (0),
    "angle" TEXT NOT NULL DEFAULT (0)
);
CREATE TABLE tbltajiparam (
    "id" INTEGER NOT NULL,
    "local" TEXT NOT NULL,
    "serial" TEXT NOT NULL,
    "x_pos" TEXT NOT NULL,
    "y_pos" TEXT NOT NULL,
    "type" TEXT NOT NULL,
    "height" TEXT NOT NULL,
    "max_car_dist" TEXT,
    "min_car_dist" TEXT,
    "long_arm_len" TEXT,
    "short_arm_len" TEXT,
    "max_up_angle" TEXT,
    "min_up_angle" TEXT,
    "jiegou" TEXT
);
