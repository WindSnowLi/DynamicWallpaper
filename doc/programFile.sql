/*
 Navicat Premium Data Transfer

 Source Server         : MessageManagement
 Source Server Type    : MySQL
 Source Server Version : 50718
 Source Host           : cdb-7qv2gt72.cd.tencentcdb.com:10107
 Source Schema         : programFile

 Target Server Type    : MySQL
 Target Server Version : 50718
 File Encoding         : 65001

 Date: 01/05/2020 20:47:45
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for programUp
-- ----------------------------
DROP TABLE IF EXISTS `programUp`;
CREATE TABLE `programUp`  (
  `filename` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `filetype` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `version` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `download` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `link` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`filename`, `version`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of programUp
-- ----------------------------
INSERT INTO `programUp` VALUES ('CRIPPLE.dll', 'run', '1.20.4.26', '\\\\CRIPPLE.dll', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicLibrary/CRIPPLE.dll');
INSERT INTO `programUp` VALUES ('DynamicWallpaper.exe', 'run', '2.20.4.30', '\\\\DynamicWallpaper.exe', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicWallpaper.exe');
INSERT INTO `programUp` VALUES ('libeay32.dll', 'run', '1.20.4.25', '\\\\libeay32.dll', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicLibrary/libeay32.dll');
INSERT INTO `programUp` VALUES ('libmysql.dll', 'run', '1.20.4.25', '\\\\libmysql.dll', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicLibrary/libmysql.dll');
INSERT INTO `programUp` VALUES ('libvlc.dll', 'run', '1.20.4.25', '\\\\libvlc.dll', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicLibrary/libvlc.dll');
INSERT INTO `programUp` VALUES ('libvlccore.dll', 'run', '1.20.4.25', '\\\\libvlccore.dll', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicLibrary/libvlccore.dll');
INSERT INTO `programUp` VALUES ('LiveUpdate.exe', 'run', '1.20.4.30', '\\\\LiveUpdate.exe', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicLibrary/LiveUpdate.exe');
INSERT INTO `programUp` VALUES ('Service.exe', 'run', '1.20.4.25', '\\\\Service.exe', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicLibrary/Service.exe');
INSERT INTO `programUp` VALUES ('SERVICEDLL.dll', 'run', '1.20.4.25', '\\\\SERVICEDLL.dll', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/DynamicLibrary/SERVICEDLL.dll');
INSERT INTO `programUp` VALUES ('VersionInformation.xml', 'config', '1.20.4.30', '\\\\config\\\\VersionInformation.xml', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/config/VersionInformation.xml');
INSERT INTO `programUp` VALUES ('VideoDirectory.xml', 'config', '1.20.4.25', '\\\\config\\\\VideoDirectory.xml', 'http://wzmmdsnn.gitee.io/windsnowli/ProgramFile/config/VideoDirectory.xml');
INSERT INTO `programUp` VALUES ('壁纸.msi', 'installationpackage', '2.20.5.1', '\\\\壁纸.msi', 'https://windsnowli.github.io/DynamicWallpaper.msi');

SET FOREIGN_KEY_CHECKS = 1;
