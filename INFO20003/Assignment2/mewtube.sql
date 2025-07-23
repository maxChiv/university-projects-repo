/* If you are running this script on your local host, uncomment the next 4 lines. */
-- DROP SCHEMA IF EXISTS `mewtube` ;
-- CREATE SCHEMA IF NOT EXISTS `mewtube` DEFAULT CHARACTER SET utf8 ;
-- USE `mewtube` ;
-- SET GLOBAL sql_mode=(SELECT CONCAT(@@sql_mode,',ANSI')); 


-- Set the correct SQL mode (disallow illegal groupbys)
SET sql_mode=(SELECT CONCAT(@@sql_mode,',ANSI')); 


-- Forward engineered

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Table `user`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `user` ;

CREATE TABLE IF NOT EXISTS `user` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `username` VARCHAR(45) NOT NULL,
  `email` VARCHAR(100) NOT NULL,
  `authType` ENUM('gmail', 'facebook', 'github', 'twitter') NOT NULL,
  `reputation` INT NOT NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `content_creator`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `content_creator` ;

CREATE TABLE IF NOT EXISTS `content_creator` (
  `id` INT NOT NULL,
  `realName` VARCHAR(100) NOT NULL,
  `screenName` VARCHAR(45) NOT NULL,
  `website` VARCHAR(100) NULL,
  `linkedUser` INT NOT NULL,
  PRIMARY KEY (`id`),
  CONSTRAINT `fk_contentcreator_linkeduser1`
    FOREIGN KEY (`linkedUser`)
    REFERENCES `user` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE UNIQUE INDEX `linkeduser_UNIQUE` ON `content_creator` (`linkedUser` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `hashtag`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `hashtag` ;

CREATE TABLE IF NOT EXISTS `hashtag` (
  `id` INT NOT NULL,
  `tag` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `content_creator_hashtag`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `content_creator_hashtag` ;

CREATE TABLE IF NOT EXISTS `content_creator_hashtag` (
  `creatorID` INT NOT NULL,
  `hashtagID` INT NOT NULL,
  PRIMARY KEY (`creatorID`, `hashtagID`),
  CONSTRAINT `fk_contentcreator_hashtag_contentcreator1`
    FOREIGN KEY (`creatorID`)
    REFERENCES `content_creator` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_contentcreator_hashtag_hashtag1`
    FOREIGN KEY (`hashtagID`)
    REFERENCES `hashtag` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_contentcreator_hashtag_hashtag1_idx` ON `content_creator_hashtag` (`hashtagID` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `video`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `video` ;

CREATE TABLE IF NOT EXISTS `video` (
  `id` INT NOT NULL,
  `title` VARCHAR(1000) NOT NULL,
  `uploaded` DATETIME NOT NULL,
  `videoURL` VARCHAR(1000) NOT NULL,
  `thumbnailURL` VARCHAR(1000) NOT NULL,
  `viewCount` BIGINT NOT NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `cocreator`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `cocreator` ;

CREATE TABLE IF NOT EXISTS `cocreator` (
  `videoID` INT NOT NULL,
  `creatorID` INT NOT NULL,
  PRIMARY KEY (`videoID`, `creatorID`),
  CONSTRAINT `fk_cocreators_contentcreator1`
    FOREIGN KEY (`creatorID`)
    REFERENCES `content_creator` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_cocreators_video1`
    FOREIGN KEY (`videoID`)
    REFERENCES `video` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_coauthors_researcher1_idx` ON `cocreator` (`creatorID` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `video_hashtag`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `video_hashtag` ;

CREATE TABLE IF NOT EXISTS `video_hashtag` (
  `videoID` INT NOT NULL,
  `hashtagID` INT NOT NULL,
  PRIMARY KEY (`videoID`, `hashtagID`),
  CONSTRAINT `fk_video_hashtag_hashtag1`
    FOREIGN KEY (`hashtagID`)
    REFERENCES `hashtag` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_video_hashtag_video1`
    FOREIGN KEY (`videoID`)
    REFERENCES `video` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_publication_has_keyword_keyword1_idx` ON `video_hashtag` (`hashtagID` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `annotation`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `annotation` ;

CREATE TABLE IF NOT EXISTS `annotation` (
  `sourceVideoID` INT NOT NULL,
  `destinationVideoID` INT NOT NULL,
  `sourceVideoTimestamp` INT NOT NULL,
  `duration` INT NOT NULL,
  `description` VARCHAR(45) NULL,
  PRIMARY KEY (`sourceVideoID`, `destinationVideoID`, `sourceVideoTimestamp`),
  CONSTRAINT `fk_annotations_sourcevideo`
    FOREIGN KEY (`sourceVideoID`)
    REFERENCES `video` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_annotations_destinationvideo`
    FOREIGN KEY (`destinationVideoID`)
    REFERENCES `video` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `rating`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `rating` ;

CREATE TABLE IF NOT EXISTS `rating` (
  `videoID` INT NOT NULL,
  `linkedUser` INT NOT NULL,
  `ratingTime` DATETIME NOT NULL,
  `rating` ENUM('dislike', 'neutral', 'like') NOT NULL,
  `comment` VARCHAR(1000) NULL,
  PRIMARY KEY (`videoID`, `linkedUser`),
  CONSTRAINT `fk_ratings_videoid1`
    FOREIGN KEY (`videoID`)
    REFERENCES `video` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_ratings_linkedaccount1`
    FOREIGN KEY (`linkedUser`)
    REFERENCES `user` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_ratings_linkedaccount_idx` ON `rating` (`linkedUser` ASC) VISIBLE;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

-- INSERT VALUES

INSERT INTO user (id, username, email, authtype, reputation)
VALUES 
    (1, 'ml5', 'michael@email.com', 'gmail', 22),
    (2, 'bb44', 'bob@email.com', 'facebook', 1),
    (3, 'je22', 'jane@email.com', 'github', 49),
    (4, 'ae5', 'alice@email.com', 'facebook', 64),
    (5, 'ls5', 'lisa@email.com', 'twitter', 10),
    (6, 'ay0', 'anthony@email.com', 'facebook', 1),
    (7, 'je1', 'joe@email.com', 'github', 99),
    (8, 'mk57', 'mark@email.com', 'github', 15),
    (9, 'tt34', 'tim@email.com', 'github', 90),
    (10, 'cc100', 'colton@email.com', 'github', 10)
;


INSERT INTO content_creator (id, realName, screenName, website, linkedUser)
VALUES 
    (1, 'michael', 'mdog', null, 1),
    (2, 'jane', 'TaylorSwiftOfficial', 'www.taylor.com', 3),
    (3, 'alice', 'wonderland', 'a.url.com', 4),
    (4, 'joe', 'Mr. Yeast', 'another.url.com', 7),
    (5, 'tim', 'INFO20003Memes', 'tim.com.au', 9)
;


INSERT INTO hashtag (id, tag)
VALUES
    (1, '#technology'),
    (2, '#tiktok-trend'),
    (3, '#relational-databases'),
    (4, '#tasty-food'),
    (5, '#memes'),
    (6, '#ridiculous-drama'),
    (7, '#dogs')
;

INSERT INTO content_creator_hashtag (creatorID, hashtagID)
VALUES
    (1,1),
    (2,1),
    (5,1),
    (1,2),
    (2,2),
    (3,2),
    (4,2),
    (5,2),
    (5,3),
    (2,4),
    (3,4),
    (4,4),
    (3,5),
    (1,5),
    (5,5),
    (2,6),
    (4,6),
    (2,7)
;

INSERT INTO video (id, title, uploaded, videoURL, thumbnailURL, viewCount)
VALUES
    (1, 'Computers do WHAT', '2023-03-02', 'imagehosting.com/kjahs','imagehosting.com/fsdf', 304520),
    (2, 'Hot take: Rust', '2023-01-06', 'imagehosting.com/fsdf','imagehosting.com/ssdfsd', 30023450),
    (3, 'Watch me dance', '2023-04-01', 'imagehosting.com/acdf', 'imagehosting.com/rwerawer',300212334500),
    (4, 'Life hacks vol 23472', '2022-01-05', 'imagehosting.com/kjahs', 'imagehosting.com/dddddd',1050),
    (5, 'You ever just...', '2022-01-07', 'imagehosting.com/wretret', 'imagehosting.com/afstdyu',1234500),
    (6, 'query optimizing LIVE', '2023-02-01', 'imagehosting.com/acsdf', 'imagehosting.com/cxzfe',300345000),
    (7, 'Yes, I ate it', '2023-01-01', 'imagehosting.com/rrr', 'imagehosting.com/ukukg',1023450),
    (8, '1000 foods you should eat every day (or else)', '2023-05-01', 'imagehosting.com/sssdasd', 'imagehosting.com/kjahs',123400),
    (9, 'I crashed my car again', '2023-01-01', 'imagehosting.com/areac', 'imagehosting.com/qq',1020),
    (10, 'Guys I finally did it', '2023-07-02', 'imagehosting.com/kjahs', 'imagehosting.com/aa',1234500),
    (11, 'Witness my POWER', '2023-02-01', 'imagehosting.com/asdasdasd', 'imagehosting.com/ytvv',502340),
    (12, 'My dog is so cute', '2023-01-06', 'imagehosting.com/ooooo', 'imagehosting.com/askjdas',332300)
;

INSERT INTO cocreator (videoID, creatorID)
VALUES
    (1 ,1),
    (1 ,2),
    (1 ,5),
    (2 ,5),
    (3 ,1),
    (3 ,2),
    (4 ,5),
    (4 ,2),
    (5 ,4),
    (5 ,2),
    (5 ,1),
    (6 ,5),
    (6 ,1),
    (7 ,2),
    (7 ,3),
    (8 ,3),
    (8 ,2),
    (8 ,4),
    (9 ,2),
    (10,1),
    (10,2),
    (10,4),
    (10,5),
    (11,2),
    (12,2)
;


INSERT INTO video_hashtag (videoID, hashtagID)
VALUES
    (1, 1),
    (1, 3),
    (2, 1),
    (2, 6),
    (2, 5),
    (3, 2),
    (4, 2),
    (4, 5),
    (5, 2),
    (5, 6),
    (6, 3),
    (7, 4),
    (7, 5),
    (7, 6),
    (8, 4),
    (8, 6),
    (9, 6),
    (10, 5),
    (10, 6),
    (11, 5),
    (12, 7)
;

INSERT INTO annotation (sourceVideoID, destinationVideoID, sourceVideoTimestamp, duration, description)
VALUES
    (1, 2, 1, 1, 'watch this one too'),
    (1, 12, 5, 5, null),
    (3, 4, 2, 9, '>>>'),
    (4, 3, 1, 10, '>>>'),
    (7, 8, 4, 7, 'lots more eating here >'),
    (11, 12, 1, 1, 'love love love'),
    (10, 12, 60, 50, 'awww')
;


INSERT INTO rating (videoID, linkedUser, ratingTime, rating, comment)
VALUES 
    (1, 1, '2023-04-01', 'like', 'well done thank you'),
    (1, 9, '2023-06-01', 'neutral', 'thank you'),
    (1, 2, '2022-03-09', 'like', 'Sweeeet'),
    (2, 1, '2023-01-07', 'dislike', 'Actually...') ,
    (3, 10, '2023-04-02', 'like', 'wow, well done') ,
    (3, 1, '2023-05-01', 'dislike', 'yawn') ,
    (4, 4, '2022-01-09', 'dislike', 'Followed instructions, burnt down house') ,
    (4, 5, '2022-01-20', 'like', 'Awesome!!1') ,
    (4, 9, '2022-01-06', 'neutral', 'Needs more superglue hacks') ,
    (5, 2, '2022-01-08', 'like', 'Sweeeet') ,
    (5, 10, '2023-02-08', 'like', 'Who is watching in 2023?') ,
    (6, 8, '2023-04-06', 'like', 'You missed a semicolon @ 3:20;'),
    (7, 2, '2023-01-02', 'like', 'Sweeeet'),
    (9, 1, '2023-01-03', 'dislike', null),
    (10,1, '2023-08-01', 'like', 'mad lad'),
    (11,2, '2023-03-01', 'like', 'Sweeeet'),
    (12,1, '2023-02-01', 'like', 'awwwwwwww, thank you for sharing this!!!'),
    (12,2, '2023-01-07', 'like', 'Sweeeet'),
    (12,3, '2023-01-07', 'dislike', null),
    (12,7, '2023-02-01', 'like', 'WHO DISLIKED THIS VIDEO???')
;
