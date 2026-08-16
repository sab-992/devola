import { Component, effect, inject, PLATFORM_ID, signal, WritableSignal } from '@angular/core';
import { AbstractControl, FormBuilder, ReactiveFormsModule, ValidationErrors, Validators } from '@angular/forms';
import { MatButtonModule } from '@angular/material/button';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatIconButton } from '@angular/material/button';
import { isPlatformBrowser } from '@angular/common';

type Mode = 'login' | 'register';
type Theme = 'light' | 'dark';

/** Cross-field validator: password and confirmPassword must match. */
function passwordsMatchValidator(control: AbstractControl): ValidationErrors | null {
    const password = control.get('password')?.value;
    const confirmPassword = control.get('confirmPassword')?.value;
    if (!password || !confirmPassword) return null;
    return password === confirmPassword ? null : { passwordMismatch: true };
}

const MIN_NAME_LENGTH: number = 2;
const MIN_PASSWORD_LENGTH: number = 8;

interface UIControllers {
    mode: WritableSignal<Mode>;
    theme: WritableSignal<Theme>;

    hideConfirmPassword: WritableSignal<boolean>;
    hideLoginPassword: WritableSignal<boolean>;
    hideRegisterPassword: WritableSignal<boolean>;
    submitting: WritableSignal<boolean>;
}


@Component({
    selector: 'app-authentication',
    standalone: true,
    imports: [
        ReactiveFormsModule,
        MatFormFieldModule,
        MatInputModule,
        MatButtonModule,
        MatIconModule,
        MatIconButton,
    ],
    templateUrl: './authentication.html',
    styleUrl: './authentication.scss',
    host: {
        '[attr.data-theme]': 'theme()',
    },
})
export class Authentication {
    private readonly m_formBuilder = inject(FormBuilder);
    private readonly m_platformID = inject(PLATFORM_ID);
    private readonly ui: UIControllers = { mode: signal<Mode>('login'),
                                           theme: signal<Theme>(this.getInitialTheme()),
                                           hideConfirmPassword: signal(true),
                                           hideLoginPassword: signal(true),
                                           hideRegisterPassword: signal(true),
                                           submitting: signal(false) };



    readonly loginForm = this.m_formBuilder.nonNullable.group({ username: ['', [Validators.required, Validators.minLength(MIN_NAME_LENGTH)]],
                                                                password: ['', [Validators.required, Validators.minLength(MIN_PASSWORD_LENGTH)]] });
    readonly registerForm = this.m_formBuilder.nonNullable.group({ username:        ['', [Validators.required, Validators.minLength(MIN_NAME_LENGTH)]],
                                                                   email:           ['', [Validators.required, Validators.email]],
                                                                   password:        ['', [Validators.required, Validators.minLength(MIN_PASSWORD_LENGTH)]],
                                                                   confirmPassword: ['', [Validators.required]],
                                                                   first_name:      ['', [Validators.required, Validators.minLength(MIN_NAME_LENGTH)]],
                                                                   last_name:       ['', [Validators.required, Validators.minLength(MIN_NAME_LENGTH)]] },
                                                                 { validators: passwordsMatchValidator });

    constructor() {
        effect(() => {
            const current = this.theme();

            if (!isPlatformBrowser(this.m_platformID))
                return;

            localStorage.setItem('devola-theme', current);
            document.documentElement.setAttribute('data-theme', current);
        });
    }

    get mode() {
        return this.ui.mode;
    }

    get theme() {
        return this.ui.theme;
    }

    get hideConfirmPassword() {
        return this.ui.hideConfirmPassword;
    }

    get hideLoginPassword() {
        return this.ui.hideLoginPassword;
    }

    get hideRegisterPassword() {
        return this.ui.hideRegisterPassword;
    }

    get submitting() {
        return this.ui.submitting;
    }

    get MIN_NAME_LENGTH() {
        return MIN_NAME_LENGTH;
    }

    get MIN_PASSWORD_LENGTH() {
        return MIN_PASSWORD_LENGTH;
    }

    switchMode(next: Mode): void {
        this.mode.set(next);
    }

    currentForm() {
        return this.mode() == "login" ? this.loginForm : this.registerForm;
    }

    toggleTheme(): void {
        this.theme.update((t) => (t === 'light' ? 'dark' : 'light'));
    }

    toggleLoginPasswordVisibility(event: Event): void {
        event.preventDefault();
        this.hideLoginPassword.update((v) => !v);
    }

    toggleRegisterPasswordVisibility(event: Event): void {
        event.preventDefault();
        this.hideRegisterPassword.update((v) => !v);
    }

    toggleConfirmPasswordVisibility(event: Event): void {
        event.preventDefault();
        this.hideConfirmPassword.update((v) => !v);
    }

    submitLogin(): void {
        if (this.loginForm.invalid) {
            this.loginForm.markAllAsTouched();
            return;
        }
        this.submitting.set(true);
        const payload = this.loginForm.getRawValue();

        // TODO: add authentication service

        console.log('Login submitted', payload);
        this.submitting.set(false);
    }

    submitRegister(): void {
        if (this.registerForm.invalid) {
            this.registerForm.markAllAsTouched();
            return;
        }

        this.submitting.set(true);
        const payload = this.registerForm.getRawValue();


        // TODO: add authentication service

        console.log('Register submitted', payload);
        this.submitting.set(false);
    }

    private getInitialTheme(): Theme {
        if (!isPlatformBrowser(this.m_platformID))
            return "light";

        const stored = localStorage.getItem('devola-theme') as Theme | null;
        if (stored === 'light' || stored === 'dark') return stored;
        return window.matchMedia?.('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
    }
}