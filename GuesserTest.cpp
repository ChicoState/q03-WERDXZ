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

// Count down utility for future extensibility
// This macro will generate two local variables:
//  object: the Guesser object being tested
//  c: the countdown
#define OBJ(data)         \
	Guesser object(data); \
	int __c = 3

// This macro will reset the local `c` variable
// to the default value of 3.
// Must be used after `OBJ` been invoked
#define RESET() __c = 3

// This macro will return the current countdown
#define CURRENT __c

// This macro will decrement the countdown by one and return it
#define DECREMENT __c - 1 < 0 ? 0 : --__c

// Test exact match returns true
TEST(GuesserTest, ExactMatch) {
	OBJ("Secret");

	EXPECT_TRUE(object.match("Secret"));
}

// Test non-matching guess returns false
TEST(GuesserTest, NoMatch) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("Wrong"));
}

// Test initial remaining guesses is 3
TEST(GuesserTest, InitialRemainingGuesses) {
	OBJ("Secret");

	EXPECT_EQ(CURRENT, object.remaining());
}

// Test remaining guesses decreases after failed attempt
TEST(GuesserTest, DecrementRemainingGuesses) {
	OBJ("Secret");

	EXPECT_EQ(CURRENT, object.remaining());
	object.match("Secres");
	EXPECT_EQ(DECREMENT, object.remaining());
}

// Test guesser locks when distance > 2
TEST(GuesserTest, LockOnDistanceGreaterThanTwo) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("Testing"));
	EXPECT_FALSE(object.match("Secret"));
}

// Test guesser fails after 3 unsuccessful attempts
TEST(GuesserTest, FailAfterThreeAttempts) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("Secre1"));
	EXPECT_FALSE(object.match("Secre2"));
	EXPECT_FALSE(object.match("Secre3"));
	EXPECT_FALSE(object.match("Secret"));
}

// Test if countdown continue after locking
TEST(GuesserTest, CountdownAfterLock) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("SecretLock"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secret"));
	EXPECT_EQ(DECREMENT, object.remaining());
}

// Test constructor truncates secret longer than 32 characters
TEST(GuesserTest, TruncateSecretLength) {
	OBJ("ThisSecretIsMuchLongerThanThirtyTwoCharactersAndShouldBeTruncated");

	EXPECT_TRUE(object.match("ThisSecretIsMuchLongerThanThirty"));
}

// Test oversized matches
TEST(GuesserTest, OversizedMatch) {
	OBJ("ThisSecretIsMuchLongerThanThirtyTwoCharactersAndShouldBeTruncate"
		"d");  // this should be truncated

	EXPECT_FALSE(
		object.match("ThisSecretIsMuchLongerThanThirtyTwoCharactersAndShouldBeT"
					 "runcated"));	// thus this should not match
									// This should be locked
	EXPECT_FALSE(object.match("ThisSecretIsMuchLongerThanThirtyT"));
}

// Test with close guesses (distance <= 2)
TEST(GuesserTest, CloseGuesses) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("Secrey"));
	EXPECT_EQ(DECREMENT, object.remaining());

	// Two characters different
	EXPECT_FALSE(object.match("Secrex"));
	EXPECT_EQ(DECREMENT, object.remaining());
}

// Test with empty string as secret
// Since this is smaller than 2 characters, the distance wouldn't go beyond 2
// and it wouldn't lock
TEST(GuesserTest, EmptySecret) {
	OBJ("");

	EXPECT_TRUE(object.match(""));
	EXPECT_FALSE(object.match("anything")); // Should not lock
	EXPECT_TRUE(object.match(""));
}

// Test case sensitivity
TEST(GuesserTest, CaseSensitivity) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("secret"));
}

// Test distance calculation for shorter guess
TEST(GuesserTest, DistanceWithShorterGuess) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("Secr"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_TRUE(object.match("Secret"));  // Test if not locked
}

// Test distance calculation for longer guess
// "If the secret is locked for any other reason, such as a big distance in
// a guess, the count of remaining guesses should still count down as usual
// to hide that the secret has been locked." (Guesser.cpp:85~87)
TEST(GuesserTest, DistanceWithLongerGuess) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("SecretWord"));
	EXPECT_FALSE(object.match("Secret"));
	EXPECT_FALSE(object.match("SecretWord"));  // Test if locked
}

// Test guesser behavior with exact distance of 2
TEST(GuesserTest, ExactDistanceOfTwo) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("SeXrXt"));
	EXPECT_FALSE(object.match("SeXrYt"));
	EXPECT_TRUE(object.match("Secret"));  // Test if not locked
}

// Test distance calculation with both different characters and length
TEST(GuesserTest, MixedDistanceCalculation) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("Secr3"));
	EXPECT_TRUE(object.match("Secret"));  // Test if not locked
}

// Test remaining guesses countdowns properly through attempts
TEST(GuesserTest, RemainingGuessesCountdown) {
	OBJ("Secret");

	EXPECT_EQ(CURRENT, object.remaining());
	EXPECT_FALSE(object.match("Secr3t"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secr3t"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secr3t"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secret"));  // Test if no attempts left
}

// Test maximum distance capping at secret length
TEST(GuesserTest, MaximumDistanceCapping) {
	OBJ("ABC");

	EXPECT_FALSE(object.match("XYZABC"));
	EXPECT_FALSE(object.match("ABC"));	// Test if locked
}

// Test boundary case of distance exactly equal to 2
TEST(GuesserTest, BoundaryDistanceCase) {
	OBJ("ABCDEF");

	EXPECT_FALSE(object.match("ABCD12"));
	EXPECT_TRUE(object.match("ABCDEF"));  // Test if not locked
}

// Test boundary case of distance exactly equal to 3
TEST(GuesserTest, BoundaryDistanceLockCase) {
	OBJ("ABCDEF");

	EXPECT_FALSE(object.match("ABC123"));
	EXPECT_FALSE(object.match("ABCDEF"));  // Test if locked
}

// Test with different character differences (positions)
TEST(GuesserTest, CharacterPositionDifferences) {
	OBJ("ABCDEF");

	EXPECT_FALSE(object.match("1BCDEF"));
	EXPECT_FALSE(object.match("A1CDEF"));
	EXPECT_FALSE(object.match("ABCDE1"));
	EXPECT_FALSE(object.match("ABCDEF"));
}

// Test with non-ascii: chinese characters
TEST(GuesserTest, NonAsciiCharacters) {
	OBJ("测试");

	EXPECT_TRUE(object.match("测试"));
	EXPECT_FALSE(object.match("测试1"));
	EXPECT_FALSE(object.match("测试2"));
	EXPECT_FALSE(object.match("测试3"));
	EXPECT_FALSE(object.match("测试"));
}

// Test non-ascii with different widths
// Each emoji should take four bytes
TEST(GuesserTest, NonAsciiWidthFour) {
	OBJ("😊😊");

	EXPECT_EQ(string("😊😊").length(), 8);
	EXPECT_TRUE(object.match("😊😊"));
	EXPECT_FALSE(object.match("😊"));
	EXPECT_FALSE(object.match("😊😊"));	 // Test if locked
}

// Each common Chinese character should take three bytes
TEST(GuesserTest, NonAsciiWidthThree) {
	OBJ("测试");
	EXPECT_EQ(string("测试").size(), 6);
	EXPECT_TRUE(object.match("测试"));
	EXPECT_FALSE(object.match("测"));
	EXPECT_FALSE(object.match("测试"));	 // Test if locked
}

// Each European character with diacritics should take two bytes
TEST(GuesserTest, NonAsciiWidthTwo) {
	OBJ("ŤĔŜŤ");

	EXPECT_EQ(string("ŤĔŜŤ").size(), 8);
	EXPECT_TRUE(object.match("ŤĔŜŤ"));
	EXPECT_FALSE(object.match("ŤĔŜ"));
	EXPECT_FALSE(object.match("ŤĔŜ"));
	EXPECT_TRUE(object.match("ŤĔŜŤ"));	// Test if not locked
}

// Test European characters with diacritics when locked
TEST(GuesserTest, NonAsciiWidthTwoLocked) {
	OBJ("ŤĔŜŤ");

	EXPECT_EQ(string("ŤĔŜŤ").size(), 8);
	EXPECT_TRUE(object.match("ŤĔŜŤ"));
	EXPECT_FALSE(object.match("ŤĔŜŤŤ"));
	EXPECT_FALSE(object.match("ŤĔŜŤŤŤ"));  // Should be locked from now on
	EXPECT_FALSE(object.match("ŤĔŜŤ"));	   // Test if locked
}

// Since max distance is the length of the secret, this tests unlimited guesses
TEST(GuesserTest, UnlimitedGuesses) {
	OBJ("ub");

	EXPECT_FALSE(object.match("114514"));
	EXPECT_FALSE(object.match("1919810"));
	EXPECT_TRUE(object.match("ub"));  // Test if not locked
}

// Test attempts reset to 3 after a successful match
TEST(GuesserTest, ResetAttemptsAfterSuccess) {
	OBJ("Secret");

	EXPECT_EQ(CURRENT, object.remaining());

	// First wrong guess
	EXPECT_FALSE(object.match("Secre1"));
	EXPECT_EQ(DECREMENT, object.remaining());

	// Correct guess - attempts should reset to 3
	EXPECT_TRUE(object.match("Secret"));
	EXPECT_EQ(RESET(), object.remaining());

	// Make two wrong guesses
	EXPECT_FALSE(object.match("Secre2"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secre3"));
	EXPECT_EQ(DECREMENT, object.remaining());

	// Correct guess
	// attempts should reset to 3 again
	EXPECT_TRUE(object.match("Secret"));
	EXPECT_EQ(RESET(), object.remaining());
}

// Test multiple cycles of attempts reset after success
TEST(GuesserTest, MultipleCyclesOfAttemptsReset) {
	OBJ("Password");

	{
		EXPECT_FALSE(object.match("Passwor1"));
		EXPECT_EQ(DECREMENT, object.remaining());
		EXPECT_TRUE(object.match("Password"));
		EXPECT_EQ(RESET(), object.remaining());	 // Reset to 3
	}

	{
		EXPECT_FALSE(object.match("Passwor2"));
		DECREMENT;
		EXPECT_FALSE(object.match("Passwor3"));
		EXPECT_EQ(DECREMENT, object.remaining());
		EXPECT_TRUE(object.match("Password"));
		EXPECT_EQ(RESET(), object.remaining());	 // Reset to 3
	}

	{
		EXPECT_FALSE(object.match("Passw0rd"));
		DECREMENT;
		EXPECT_FALSE(object.match("Passw1rd"));
		DECREMENT;
		EXPECT_FALSE(object.match("Passw2rd"));
		EXPECT_EQ(DECREMENT, object.remaining());
	}

	// Locked due to running out of attempts
	EXPECT_FALSE(object.match("Password"));	 // Would match but we're locked
	EXPECT_EQ(0, object.remaining());
}

// Test attempts do not reset after success when locked due to distance > 2
TEST(GuesserTest, NoResetWhenLockedDueToDistance) {
	OBJ("Password");

	// Lock due to distance > 2
	EXPECT_FALSE(object.match("12345678"));
	EXPECT_EQ(2, object.remaining());

	// Try correct match, should still be locked
	EXPECT_FALSE(object.match("Password"));
	EXPECT_EQ(1, object.remaining());  // Count still decreases
}

// Test no underflow
TEST(GuesserTest, NoUnderflow) {
	OBJ("Secret");

	EXPECT_FALSE(object.match("Secre1"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secre2"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secre3"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secre4"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secre5"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secre6"));
	EXPECT_EQ(DECREMENT, object.remaining());
	EXPECT_FALSE(object.match("Secre7"));
	EXPECT_EQ(DECREMENT, object.remaining());
}
