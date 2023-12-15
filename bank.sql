
SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

CREATE TABLE `account_holder`(
  `account_no` int(10) UNSIGNED ZEROFILL NOT NULL,
  `user_name` varchar(255) NOT NULL,
  `user_email` varchar(255) NOT NULL,
  `aadhar_no` int(15) NOT NULL,
  `total_balance` int(11) NOT NULL,
  `user_pwd` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE `transhis` (
  `tid` int(11) NOT NULL,
  `tamt` int(11) NOT NULL,
  `tstatus` varchar(100) NOT NULL,
  `tdate` varchar(200) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

ALTER TABLE `account_holder`
  ADD PRIMARY KEY (`account_no`);

ALTER TABLE `transhis`
  ADD KEY `tid` (`tid`);

COMMIT;
