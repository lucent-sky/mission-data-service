# Mission Data Service
# Software Test Plan

## 1. Test Strategy

Testing includes:
- Unit tests for validation logic
- Manual integration testing using curl
- Requirement traceability mapping

---

## 2. Test Environment

- Ubuntu 22.04
- g++ 11+
- Docker 24+
- C++17

---

## 3. Requirement Traceability Matrix

| Test ID | Requirement | Description |
|---------|-------------|------------|
| TEST-1  | REQ-1 | Verify POST /ingest is reachable |
| TEST-2  | REQ-4 | Verify malformed JSON returns 400 |
| TEST-3  | REQ-5 | Verify empty source_id rejected |
| TEST-4  | REQ-6 | Verify valid record stored |
| TEST-5  | REQ-7 | Verify GET /stats reachable |
| TEST-6  | REQ-8 | Verify stats correctness |
| TEST-7  | REQ-9 | Verify 204 when no records |
| TEST-8  | REQ-11 | Verify graceful shutdown |

---

## 4. Unit Tests  

### TEST-UT-1
Input: Valid DataRecord  
Expected: Validation succeeds

### TEST-UT-2
Input: Empty source_id  
Expected: Validation fails

### TEST-UT-3
Input: value = NaN  
Expected: Validation fails

---

## 5. Manual Integration Tests

### TEST-INT-1
Command:
curl -X POST localhost:8080/ingest \
-H "Content-Type: application/json" \
-d '{"source_id":"sensor_1","value":10.5,"timestamp":1700000000}'

Expected: HTTP 200

---

### TEST-INT-2
Malformed JSON:
curl -X POST localhost:8080/ingest -d '{bad}'

Expected: HTTP 400

---

### TEST-INT-3
Retrieve Stats:
curl localhost:8080/stats

Expected JSON:
{
  "count": 1,
  "min": 10.5,
  "max": 10.5,
  "average": 10.5
}

---

## 6. Acceptance Criteria

The system shall be considered complete when:
- All functional requirements pass associated tests
- Docker container builds successfully
- Service runs as non-root user
- No compiler warnings present

---

## 7. Future Testing Improvements

- Automated CI pipeline
- Static analysis integration
- Load testing
- Fuzz testing of JSON input
