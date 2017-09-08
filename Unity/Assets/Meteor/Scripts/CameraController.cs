using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 	Camera Rig
// 		Pivot
// 			Camera

public class CameraController : MonoBehaviour 
{
	public Transform m_Target;

	protected Transform m_Cam; // the transform of the camera
	protected Transform m_Pivot; // the point at which the camera pivots around
	protected Vector3 m_LastTargetPosition;

	
	private float m_MoveSpeed = 1f;                      // How fast the rig will move to keep up with the target's position.
	private float m_TurnSpeed = 1.5f;   // How fast the rig will rotate from user input.
	private float m_TurnSmoothing = 0.0f;                // How much smoothing to apply to the turn input, to reduce mouse-turn jerkiness
	private float m_TiltMax = 75f;                       // The maximum value of the x axis rotation of the pivot.
	private float m_TiltMin = 45f;                       // The minimum value of the x axis rotation of the pivot.
	private bool m_VerticalAutoReturn = false;           // set wether or not the vertical axis should auto return

	private float m_LookAngle;                    // The rig's y axis rotation.
	private float m_TiltAngle;                    // The pivot's x axis rotation.
	private const float k_LookDistance = 100f;    // How far in front of the pivot the character's look target is.
	private Vector3 m_PivotEulers;
	private Quaternion m_PivotTargetRot;
	private Quaternion m_TransformTargetRot;

	void Awake()
	{
		m_Cam = GetComponentInChildren<Camera>().transform;
		m_Pivot = m_Cam.parent;

		m_PivotEulers = m_Pivot.rotation.eulerAngles;

		m_PivotTargetRot = m_Pivot.transform.localRotation;
		m_TransformTargetRot = transform.localRotation;

		m_MoveSpeed = 300.0f;
	}

	void Start () 
	{
		var targetObj = GameObject.FindGameObjectWithTag("Player");
		m_Target = targetObj.transform;
	}

	void LateUpdate()
	{
		HandleRotationMovement();
		FollowTarget(Time.deltaTime);
	}

	private void HandleRotationMovement()
	{
		if (m_Target == null)
			return;

		if (Time.timeScale < float.Epsilon)
			return;

		// Read the user input
		var x = Input.GetAxis("Mouse X");
		var y = Input.GetAxis("Mouse Y");

		XiaoMeng xmCom = m_Target.GetComponent<XiaoMeng>();
		if (xmCom.GetIsLockHView())
		{
			x = 0.0f;
		}

		// Adjust the look angle by an amount proportional to the turn speed and horizontal input.
		m_LookAngle += x * m_TurnSpeed;

		// Rotate the rig (the root object) around Y axis only:
		m_TransformTargetRot = Quaternion.Euler(0f, m_LookAngle, 0f);

		if (m_VerticalAutoReturn)
		{
			// For tilt input, we need to behave differently depending on whether we're using mouse or touch input:
			// on mobile, vertical input is directly mapped to tilt value, so it springs back automatically when the look input is released
			// we have to test whether above or below zero because we want to auto-return to zero even if min and max are not symmetrical.
			m_TiltAngle = y > 0 ? Mathf.Lerp(0, -m_TiltMin, y) : Mathf.Lerp(0, m_TiltMax, -y);
		}
		else
		{
			// on platforms with a mouse, we adjust the current angle based on Y mouse input and turn speed
			m_TiltAngle -= y * m_TurnSpeed;
			// and make sure the new value is within the tilt range
			m_TiltAngle = Mathf.Clamp(m_TiltAngle, -m_TiltMin, m_TiltMax);
		}

		// Tilt input around X is applied to the pivot (the child of this object)
		m_PivotTargetRot = Quaternion.Euler(m_TiltAngle, m_PivotEulers.y, m_PivotEulers.z);

		if (m_TurnSmoothing > 0)
		{
			m_Pivot.localRotation = Quaternion.Slerp(m_Pivot.localRotation, m_PivotTargetRot, m_TurnSmoothing * Time.deltaTime);
			transform.localRotation = Quaternion.Slerp(transform.localRotation, m_TransformTargetRot, m_TurnSmoothing * Time.deltaTime);
		}
		else
		{
			m_Pivot.localRotation = m_PivotTargetRot;
			transform.localRotation = m_TransformTargetRot;
		}
	}

	public void FollowTarget(float deltaTime)
	{
		if (m_Target == null) return;
		transform.position = Vector3.Lerp(transform.position, m_Target.position, deltaTime * m_MoveSpeed);
	}
}
