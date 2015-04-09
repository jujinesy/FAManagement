-- MySQL dump 10.13  Distrib 5.1.69, for redhat-linux-gnu (i386)
--
-- Host: localhost    Database: FAdb
-- ------------------------------------------------------
-- Server version	5.1.69

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `FILE_LOG`
--

DROP TABLE IF EXISTS `FILE_LOG`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `FILE_LOG` (
  `USER_ID` varchar(50) DEFAULT NULL,
  `EVENT_TYPE` varchar(7) DEFAULT NULL,
  `EVENT_TIME` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `OLD_FILE_PATH` varchar(500) DEFAULT NULL,
  `OLD_FILE_NAME` varchar(250) DEFAULT NULL,
  `NEW_FILE_PATH` varchar(500) DEFAULT NULL,
  `NEW_FILE_NAME` varchar(250) DEFAULT NULL,
  `READONLY` varchar(3) DEFAULT NULL,
  `HIDDEN` varchar(3) DEFAULT NULL,
  `SYSTEM` varchar(3) DEFAULT NULL,
  `DIRECTORY` varchar(3) DEFAULT NULL,
  `ARCHIVE` varchar(3) DEFAULT NULL,
  `DEVICE` varchar(3) DEFAULT NULL,
  `NORMAL` varchar(3) DEFAULT NULL,
  `TEMPORARY` varchar(3) DEFAULT NULL,
  `SPARSE_FILE` varchar(3) DEFAULT NULL,
  `REPARSE_POINT` varchar(3) DEFAULT NULL,
  `COMPRESSED` varchar(3) DEFAULT NULL,
  `OFFLINE` varchar(3) DEFAULT NULL,
  `NOT_CONTENT_INDEXED` varchar(3) DEFAULT NULL,
  `ENCRYPTED` varchar(3) DEFAULT NULL,
  `INTEGRITY_STREAM` varchar(3) DEFAULT NULL,
  `VIRTUAL` varchar(3) DEFAULT NULL,
  `NO_SCRUB_DATA` varchar(3) DEFAULT NULL,
  `MD5_HASH` varchar(200) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `FILE_LOG`
--

LOCK TABLES `FILE_LOG` WRITE;
/*!40000 ALTER TABLE `FILE_LOG` DISABLE KEYS */;
INSERT INTO `FILE_LOG` VALUES ('sdaasd','RENAME','2013-10-31 22:05:04','D:\\1\\커넥터쓰레기.txt','커넥터쓰레기.txt','D:\\1\\쓰레기.txt','쓰레기.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('sdaasd','CHANGED','2013-10-31 22:05:04','','','D:\\1\\쓰레기.txt','쓰레기.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-10-31 22:12:43','D:\\1\\','','D:\\1\\','','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-10-31 22:12:43','','','D:\\1\\','','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CREATED','2013-11-03 17:56:31','','','D:\\1\\새 텍스트 문서.txt','새 텍스트 문서.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-03 17:56:37','D:\\1\\새 텍스트 문서.txt','새 텍스트 문서.txt','D:\\1\\hagul 1818.txt','hagul 1818.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-03 17:56:37','','','D:\\1\\hagul 1818.txt','hagul 1818.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-03 17:56:42','','','D:\\1\\hagul 1818.txt','hagul 1818.txt','NO','YES','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-03 17:56:45','','','D:\\1\\hagul 1818.txt','hagul 1818.txt','YES','YES','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-03 17:57:27','D:\\1\\gsfg.txt','gsfg.txt','D:\\1\\한글시잘놈아.txt','한글시잘놈아.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-03 17:57:27','','','D:\\1\\한글시잘놈아.txt','한글시잘놈아.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CREATED','2013-11-03 18:07:50','','','D:\\1\\종다리','종다리','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-03 18:07:50','D:\\1\\종다리','종다리','D:\\1\\54646','54646','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('sdsdf','CREATED','2013-11-04 05:05:51','','','D:\\1\\찌르레기','찌르레기','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('sdfsdf','CREATED','2013-11-04 07:00:10','','','D:\\1\\독수리','독수리','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('sdfsdf','CREATED','2013-11-04 07:01:13','','','D:\\1\\메추라기','메추라기','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('sdfsdf','RENAME','2013-11-04 07:01:14','D:\\1\\메추라기','메추라기','D:\\1\\sdfasdf','sdfasdf','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-04 07:03:55','D:\\1\\','','D:\\1\\','','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-04 07:03:55','','','D:\\1\\','','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-04 07:15:21','D:\\1\\한글시잘23놈아.txt','한글시잘23놈아.txt','D:\\1\\e한글.txt','e한글.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-04 07:15:21','','','D:\\1\\e한글.txt','e한글.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-04 07:18:06','D:\\1\\e한글.txt','e한글.txt','D:\\1\\라핳.txt','라핳.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-04 07:18:06','','','D:\\1\\라핳.txt','라핳.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-04 07:21:50','D:\\1\\라핳.txt','라핳.txt','D:\\1\\ㄷㄱㄷㄱ.txt','ㄷㄱㄷㄱ.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-04 07:21:50','','','D:\\1\\ㄷㄱㄷㄱ.txt','ㄷㄱㄷㄱ.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-04 07:24:25','D:\\1\\ggg.txt','ggg.txt','D:\\1\\3.txt','3.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO',''),('','CHANGED','2013-11-04 07:24:25','','','D:\\1\\3.txt','3.txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO',''),('','RENAME','2013-11-04 07:29:37','D:\\1\\?룔꽦?룔꽦.txt','?룔꽦?룔꽦.txt','D:\\1\\?뉎꽩?밤뀋??txt','?뉎꽩?밤뀋??txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','한글'),('','CHANGED','2013-11-04 07:29:37','','','D:\\1\\?뉎꽩?밤뀋??txt','?뉎꽩?밤뀋??txt','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','한글'),('','CREATED','2013-11-09 11:34:08','','','D:\\1\\올빼미','올빼미','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CREATED','2013-11-09 11:45:52','','','D:\\1\\올빼미\\새 비트맵 이미지.bmp','새 비트맵 이미지.bmp','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-09 11:45:52','','','D:\\1\\올빼미','올빼미','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','RENAME','2013-11-09 11:45:56','D:\\1\\올빼미\\새 비트맵 이미지.bmp','새 비트맵 이미지.bmp','D:\\1\\올빼미\\이름변경.bmp','이름변경.bmp','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-09 11:45:56','','','D:\\1\\올빼미','올빼미','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-09 11:45:56','','','D:\\1\\올빼미\\이름변경.bmp','이름변경.bmp','NO','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-09 11:46:01','','','D:\\1\\올빼미\\이름변경.bmp','이름변경.bmp','NO','YES','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5'),('','CHANGED','2013-11-09 11:46:19','','','D:\\1\\올빼미\\이름변경.bmp','이름변경.bmp','YES','NO','NO','NO','YES','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','NO','md5');
/*!40000 ALTER TABLE `FILE_LOG` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `NOTICE`
--

DROP TABLE IF EXISTS `NOTICE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `NOTICE` (
  `IDX` int(11) NOT NULL AUTO_INCREMENT,
  `NOTICE` varchar(3) DEFAULT NULL,
  `USER_ID` varchar(50) DEFAULT NULL,
  `TITLE` varchar(50) DEFAULT NULL,
  `CONTENT` varchar(200) DEFAULT NULL,
  `SEND_TIME` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`IDX`)
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `NOTICE`
--

LOCK TABLES `NOTICE` WRITE;
/*!40000 ALTER TABLE `NOTICE` DISABLE KEYS */;
INSERT INTO `NOTICE` VALUES (2,'YES','','한글','감사합니다','2013-11-09 10:38:01'),(3,'NO','aaa','특정아이디 공지','감사합니다','2013-11-09 10:39:02'),(4,'YES','','한글 전체공지2222','감사합니다','2013-11-09 16:45:47'),(5,'YES','','한글 전체공지2222그래 전체지','감사합니다','2013-11-09 19:03:25'),(6,'aaa','','특정 유저 공지2222그래 전체지','감사합니다','2013-11-09 19:03:59'),(7,'NO','aaa1','특정 유저 공지2222그래 전체지','감사합니다','2013-11-09 19:04:15'),(8,'NOE','USER_ID','TITLE','CONTENT','2013-11-14 23:18:31'),(9,'NO','AA','ㅁㅁ','ㄹㄹAA','2013-11-14 23:21:45'),(10,'NO','','','','2013-11-14 23:22:52'),(11,'YES','','오늘 뭐할까>?','그래.,.\r\n할게없어용','2013-11-14 23:31:41'),(12,'YES','','한글도 잘되고','ㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋ','2013-11-15 03:14:06'),(13,'NO','ㅁㅁ','ㅁㅁㅁ','ㅁㅁㅁㅁㅁㅁㅁㅁ','2013-11-15 03:14:16');
/*!40000 ALTER TABLE `NOTICE` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `NOTICE_CHECK`
--

DROP TABLE IF EXISTS `NOTICE_CHECK`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `NOTICE_CHECK` (
  `IDX` int(11) DEFAULT NULL,
  `USER_ID` varchar(50) DEFAULT NULL,
  `CHECK_TIME` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `NOTICE_CHECK`
--

LOCK TABLES `NOTICE_CHECK` WRITE;
/*!40000 ALTER TABLE `NOTICE_CHECK` DISABLE KEYS */;
INSERT INTO `NOTICE_CHECK` VALUES (2,'aa','2013-11-09 18:31:14'),(4,'aa','2013-11-09 18:31:28'),(4,'aa','2013-11-09 18:33:44'),(2,'','2013-11-09 18:44:05'),(4,'','2013-11-09 18:44:05'),(4,'asass','2013-11-09 18:44:34'),(4,'asass88','2013-11-09 18:45:45'),(2,'asass88','2013-11-09 18:45:46'),(2,'asass88sdf','2013-11-09 18:48:20'),(4,'asass88sdf','2013-11-09 18:48:30'),(4,'aaa1','2013-11-09 19:02:49'),(2,'aaa1','2013-11-09 19:02:50'),(5,'aaa1','2013-11-09 19:03:42'),(7,'aaa1','2013-11-09 19:04:26'),(5,'aaav12','2013-11-09 19:21:24'),(4,'aaav12','2013-11-09 19:21:25'),(2,'aaav12','2013-11-09 19:21:25'),(2,'sdf','2013-11-09 19:28:11'),(4,'sdf','2013-11-09 19:28:14'),(5,'sdf','2013-11-09 19:28:15'),(5,'a002','2013-11-09 21:04:34'),(4,'a002','2013-11-09 21:04:34'),(2,'a002','2013-11-09 21:04:35'),(5,'a002','2013-11-09 21:04:37'),(4,'a002','2013-11-09 21:04:38'),(2,'a002','2013-11-09 21:04:38'),(4,'a002','2013-11-09 21:04:40'),(5,'a002','2013-11-09 21:04:40'),(5,'a002','2013-11-09 21:04:41'),(5,'a002','2013-11-09 21:04:41'),(5,'a002','2013-11-09 21:04:41'),(5,'a002','2013-11-09 21:04:42'),(2,'a002','2013-11-09 21:05:45'),(4,'a002','2013-11-09 21:05:45'),(5,'a002','2013-11-09 21:05:46'),(2,'a005','2013-11-10 00:51:57'),(4,'a005','2013-11-10 00:51:59'),(5,'a005','2013-11-10 00:52:10'),(4,'a005','2013-11-10 00:52:11'),(2,'a005','2013-11-10 00:52:11'),(4,'a005','2013-11-10 00:52:12'),(5,'a005','2013-11-10 00:52:12');
/*!40000 ALTER TABLE `NOTICE_CHECK` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-11-16  5:15:50
