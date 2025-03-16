#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]

}

@test "Basic external command execution" {
    run ./dsh <<EOF
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Command with arguments" {
    run ./dsh <<EOF
echo "hello world"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

@test "CD command - no args" {
    run ./dsh <<EOF
pwd
cd
pwd
EOF
    [ "$status" -eq 0 ]
    # Both pwd outputs should be identical since cd with no args does nothing
    [ "$(echo "$output" | grep -c "$(pwd)")" -eq 2 ]
}

@test "CD command - with path" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/tmp" ]]
}

@test "Quoted arguments handling" {
    run ./dsh <<EOF
echo "  hello    world  "
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "  hello    world  " ]]
}

@test "Return code for non-existent command" {
    run ./dsh <<EOF
nonexistentcommand
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Command not found in PATH" ]]
    [[ "$output" =~ "2" ]]  # ENOENT is usually 2
}

@test "Return code for successful command" {
    run ./dsh <<EOF
echo hello
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "0" ]]  # Should show successful execution
}

@test "Handle multiple spaces between arguments" {
    run ./dsh <<EOF
echo hello      world
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

@test "Handle empty command" {
    run ./dsh <<EOF

EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "warning: no commands provided" ]]
}

@test "Handle invalid directory in cd" {
    run ./dsh <<EOF
cd /nonexistent_directory
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "No such file or directory" ]]
    [[ "$output" =~ "2" ]]  # ENOENT
}

@test "Handle mixed quotes" {
    run ./dsh <<EOF
echo "hello 'world'"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello 'world'" ]]
}

@test "Handle command with many arguments" {
    run ./dsh <<EOF
echo 1 2 3 4 5 6 7 8 9 10
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "1 2 3 4 5 6 7 8 9 10" ]]
}

@test "Exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}