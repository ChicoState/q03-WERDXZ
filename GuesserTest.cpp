/**
 * Unit Tests for the Guesser class
 **/

#include <gtest/gtest.h>

#include "Guesser.h"

class GuesserTest : public ::testing::Test {
   protected:
	GuesserTest() {}			// constructor runs before each test
	virtual ~GuesserTest() {}	// destructor cleans up after tests
	virtual void SetUp() {}		// sets up before each test (after constructor)
	virtual void TearDown() {}	// clean up after each test, (before destructor)
};

// Test exact match returns true
TEST(GuesserTest, ExactMatch) {
	Guesser object("Secret");
	EXPECT_TRUE(object.match("Secret"));
}

// Test non-matching guess returns false
TEST(GuesserTest, NoMatch) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("Wrong"));
}

// Test initial remaining guesses is 3
TEST(GuesserTest, InitialRemainingGuesses) {
	Guesser object("Secret");
	EXPECT_EQ(3, object.remaining());
}

// Test remaining guesses decreases after failed attempt
TEST(GuesserTest, DecrementRemainingGuesses) {
	Guesser object("Secret");
	EXPECT_EQ(3, object.remaining());
	object.match("Secres");
	EXPECT_EQ(2, object.remaining());
}

// Test guesser locks when distance > 2
TEST(GuesserTest, LockOnDistanceGreaterThanTwo) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("Testing"));
	EXPECT_FALSE(object.match("Secret"));
}

// Test guesser fails after 3 unsuccessful attempts
TEST(GuesserTest, FailAfterThreeAttempts) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("Secre1"));
	EXPECT_FALSE(object.match("Secre2"));
	EXPECT_FALSE(object.match("Secre3"));
	EXPECT_FALSE(object.match("Secret"));
}

// Test constructor truncates secret longer than 32 characters
TEST(GuesserTest, TruncateSecretLength) {
	Guesser object(
		"ThisSecretIsMuchLongerThanThirtyTwoCharactersAndShouldBeTruncated");
	EXPECT_TRUE(object.match("ThisSecretIsMuchLongerThanThirty"));
}

// Test oversized matches
TEST(GuesserTest, OversizedMatch) {
	Guesser object(
		"ThisSecretIsMuchLongerThanThirtyTwoCharactersAndShouldBeTruncate"
		"d");  // this should be truncated
	EXPECT_FALSE(
		object.match("ThisSecretIsMuchLongerThanThirtyTwoCharactersAndShouldBeT"
					 "runcated"));	// thus this should not match
	// since locked, remaining attempts SHOULD decrement.
	// "If the secret is locked for any other reason, such as a big distance in
	// a guess, the count of remaining guesses should still count down as usual
	// to hide that the secret has been locked." (Guesser.cpp:85~87)
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("ThisSecretIsMuchLongerThanThirtyT"));
	EXPECT_EQ(1, object.remaining());
}

// Test with close guesses (distance <= 2)
TEST(GuesserTest, CloseGuesses) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("Secrey"));
	EXPECT_EQ(2, object.remaining());

	// Two characters different
	EXPECT_FALSE(object.match("Secrex"));
	EXPECT_EQ(1, object.remaining());
}

// Test with empty string as secret
// Since this is smaller than 2 characters, the distance wouldn't go beyond 2
// and it wouldn't lock
TEST(GuesserTest, EmptySecret) {
	Guesser object("");
	EXPECT_TRUE(object.match(""));
	EXPECT_FALSE(object.match("anything"));
	EXPECT_TRUE(object.match(""));
}

// Test case sensitivity
TEST(GuesserTest, CaseSensitivity) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("secret"));
}

// Test distance calculation for shorter guess
TEST(GuesserTest, DistanceWithShorterGuess) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("Secr"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_TRUE(object.match("Secret")); // Test if not locked
}

// Test distance calculation for longer guess
// "If the secret is locked for any other reason, such as a big distance in
// a guess, the count of remaining guesses should still count down as usual
// to hide that the secret has been locked." (Guesser.cpp:85~87)
TEST(GuesserTest, DistanceWithLongerGuess) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("SecretWord"));
	EXPECT_FALSE(object.match("Secret"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("SecretWord")); // Test if locked
}

// Test guesser behavior with exact distance of 2
TEST(GuesserTest, ExactDistanceOfTwo) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("SeXrXt"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("SeXrYt"));
	EXPECT_EQ(1, object.remaining());
	EXPECT_TRUE(object.match("Secret")); // Test if not locked
}

// Test distance calculation with both different characters and length
TEST(GuesserTest, MixedDistanceCalculation) {
	Guesser object("Secret");
	EXPECT_FALSE(object.match("Secr3"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_TRUE(object.match("Secret")); // Test if not locked
}

// Test remaining guesses countdowns properly through attempts
TEST(GuesserTest, RemainingGuessesCountdown) {
	Guesser object("Secret");
	EXPECT_EQ(3, object.remaining());
	EXPECT_FALSE(object.match("Secr3t"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("Secr3t"));
	EXPECT_EQ(1, object.remaining());
	EXPECT_FALSE(object.match("Secr3t"));
	EXPECT_EQ(0, object.remaining());
	EXPECT_FALSE(object.match("Secret")); // Test if locked
}

// Test maximum distance capping at secret length
TEST(GuesserTest, MaximumDistanceCapping) {
	Guesser object("ABC");
	EXPECT_FALSE(object.match("XYZABC"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("ABC")); // Test if locked
	EXPECT_EQ(1, object.remaining());
}

// Test boundary case of distance exactly equal to 2
TEST(GuesserTest, BoundaryDistanceCase) {
	Guesser object("ABCDEF");
	EXPECT_FALSE(object.match("ABCD12"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_TRUE(object.match("ABCDEF")); // Test if not locked
}

// Test boundary case of distance exactly equal to 3
TEST(GuesserTest, BoundaryDistanceLockCase) {
	Guesser object("ABCDEF");
	EXPECT_FALSE(object.match("ABC123"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("ABCDEF")); // Test if locked
}

// Test with different character differences (positions)
TEST(GuesserTest, CharacterPositionDifferences) {
	Guesser object("ABCDEF");
	EXPECT_FALSE(object.match("1BCDEF"));
	EXPECT_FALSE(object.match("A1CDEF"));
	EXPECT_FALSE(object.match("ABCDE1"));
	EXPECT_FALSE(object.match("ABCDEF"));
}

// Test with non-ascii: chinese characters
TEST(GuesserTest, NonAsciiCharacters) {
	Guesser object("测试");
	EXPECT_TRUE(object.match("测试"));
	EXPECT_FALSE(object.match("测试1"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("测试2"));
	EXPECT_EQ(1, object.remaining());
	EXPECT_FALSE(object.match("测试3"));
	EXPECT_EQ(0, object.remaining());
	EXPECT_FALSE(object.match("测试4"));
}

// Test non-ascii with different widths
// Each emoji should take four bytes
TEST(GuesserTest, NonAsciiWidthFour) {
	Guesser object("😊😊");
	EXPECT_EQ(string("😊😊").length(), 8);
	EXPECT_TRUE(object.match("😊😊"));
	EXPECT_FALSE(object.match("😊"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("😊😊")); // Test if locked
}

// Each common Chinese character should take three bytes
TEST(GuesserTest, NonAsciiWidthThree) {
	Guesser object("测试");
	EXPECT_EQ(string("测试").size(), 6);
	EXPECT_TRUE(object.match("测试"));
	EXPECT_FALSE(object.match("测"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("测试")); // Test if locked
}

// Each European character with diacritics should take two bytes
TEST(GuesserTest, NonAsciiWidthTwo) {
	Guesser object("ŤĔŜŤ");
	EXPECT_EQ(string("ŤĔŜŤ").size(), 8);
	EXPECT_TRUE(object.match("ŤĔŜŤ"));
	EXPECT_FALSE(object.match("ŤĔŜ"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("ŤĔŜ"));
	EXPECT_EQ(1, object.remaining());
	EXPECT_TRUE(object.match("ŤĔŜŤ")); // Test if not locked
}

// Test European characters with diacritics when locked
TEST(GuesserTest, NonAsciiWidthTwoLocked) {
	Guesser object("ŤĔŜŤ");
	EXPECT_EQ(string("ŤĔŜŤ").size(), 8);
	EXPECT_TRUE(object.match("ŤĔŜŤ"));
	EXPECT_FALSE(object.match("ŤĔŜŤŤ"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("ŤĔŜŤŤŤ")); // Should be locked from now on
	EXPECT_EQ(1, object.remaining());
	EXPECT_FALSE(object.match("ŤĔŜŤ")); // Test if locked
}

// Since max distance is the length of the secret, this tests unlimited guesses
TEST(GuesserTest, UnlimitedGuesses) {
	Guesser object("ub");
	EXPECT_FALSE(object.match("114514"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("1919810"));
	EXPECT_EQ(1, object.remaining());
	EXPECT_TRUE(object.match("ub")); // Test if not locked
}

// Test attempts reset to 3 after a successful match
TEST(GuesserTest, ResetAttemptsAfterSuccess) {
	Guesser object("Secret");
	EXPECT_EQ(3, object.remaining());
	
	// First wrong guess
	EXPECT_FALSE(object.match("Secre1"));
	EXPECT_EQ(2, object.remaining());
	
	// Correct guess - attempts should reset to 3
	EXPECT_TRUE(object.match("Secret"));
	EXPECT_EQ(3, object.remaining());
	
	// Make two wrong guesses
	EXPECT_FALSE(object.match("Secre2"));
	EXPECT_EQ(2, object.remaining());
	EXPECT_FALSE(object.match("Secre3"));
	EXPECT_EQ(1, object.remaining());
	
	// Correct guess
    // attempts should reset to 3 again
	EXPECT_TRUE(object.match("Secret"));
	EXPECT_EQ(3, object.remaining());
}

// Test multiple cycles of attempts reset after success
TEST(GuesserTest, MultipleCyclesOfAttemptsReset) {
	Guesser object("Password");
	
    {
        EXPECT_FALSE(object.match("Passwor1"));
        EXPECT_EQ(2, object.remaining());
        EXPECT_TRUE(object.match("Password"));
        EXPECT_EQ(3, object.remaining()); // Reset to 3
    }
	
    {
        EXPECT_FALSE(object.match("Passwor2"));
        EXPECT_FALSE(object.match("Passwor3"));
        EXPECT_EQ(1, object.remaining());
        EXPECT_TRUE(object.match("Password"));
        EXPECT_EQ(3, object.remaining()); // Reset to 3
    }
	
    {
        EXPECT_FALSE(object.match("Passw0rd"));
        EXPECT_FALSE(object.match("Passw1rd"));
        EXPECT_FALSE(object.match("Passw2rd"));
        EXPECT_EQ(0, object.remaining());
    }

	// Locked due to running out of attempts
	EXPECT_FALSE(object.match("Password")); // Would match but we're locked
	EXPECT_EQ(0, object.remaining());
}

// Test attempts do not reset after success when locked due to distance > 2
TEST(GuesserTest, NoResetWhenLockedDueToDistance) {
	Guesser object("Password");
	
	// Lock due to distance > 2
	EXPECT_FALSE(object.match("12345678"));
	EXPECT_EQ(3, object.remaining());
	
	// Try correct match, should still be locked
	EXPECT_FALSE(object.match("Password"));
	EXPECT_EQ(2, object.remaining()); // Count still decreases
}
