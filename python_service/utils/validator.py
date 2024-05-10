def validate_request_dict(input_dict):
    required_keys = ['operation', 'uuid', 'args', 'service_name', 'function_name']
    required_types = {
        'operation': str,
        'uuid': str,
        'args': list,
        'service_name': str,
        'function_name': str
    }

    # Check if all required keys are present
    if not all(key in input_dict for key in required_keys):
        return False

    # Check if the types of the required keys match
    for key, expected_type in required_types.items():
        if not isinstance(input_dict[key], expected_type):
            return False

    return True
