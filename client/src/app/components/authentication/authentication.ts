import { Component, inject, signal, WritableSignal } from '@angular/core';
import { AbstractControl, FormBuilder, ReactiveFormsModule, ValidationErrors, Validators } from '@angular/forms';
import { MatButtonModule } from '@angular/material/button';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatIconButton } from '@angular/material/button';
import { ThemeService } from '@services/theme/theme';
import { ThemeComponent } from '@components/theme/theme';
import { UserService } from '@services/user/user';
import { HttpErrorResponse } from '@angular/common/http';
import { SECOND_IN_MS } from '@utility/settings';


type Mode = 'login' | 'register';

function passwordsMustMatch(control: AbstractControl): ValidationErrors | null {
    const password = control.get('password');
    const confirmPassword = control.get('confirmPassword');

    if (!password || !confirmPassword) // Page or Form is not loaded yet.
        return null;

    if (password.value !== confirmPassword.value) {
        confirmPassword.setErrors({ passwordMismatch: true });
        return { passwordMismatch: true };
    }

    if (confirmPassword.hasError('passwordMismatch'))
        confirmPassword.setErrors(null);

    return null;
}

const MIN_NAME_LENGTH: number = 2;
const MIN_PASSWORD_LENGTH: number = 8;

interface UIControllers {
    mode: WritableSignal<Mode>;

    hideConfirmPassword: WritableSignal<boolean>;
    hideLoginPassword: WritableSignal<boolean>;
    hideRegisterPassword: WritableSignal<boolean>;
    incorrectCredentials: WritableSignal<boolean>;
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
        ThemeComponent
    ],
    templateUrl: './authentication.html',
    styleUrl: './authentication.scss',
    host: {
        '[attr.data-theme]': 'theme()',
    },
})
export class AuthenticationPage {
    private readonly m_userService = inject(UserService);
    private readonly m_formBuilder = inject(FormBuilder);
    private readonly ui: UIControllers = { hideConfirmPassword:  signal(true),
                                           hideLoginPassword:    signal(true),
                                           hideRegisterPassword: signal(true),
                                           incorrectCredentials: signal(false),
                                           mode:                 signal<Mode>('login'),
                                           submitting:           signal(false) };

    readonly loginForm = this.m_formBuilder.nonNullable.group({ username: ['', [Validators.required, Validators.minLength(MIN_NAME_LENGTH)]],
                                                                password: ['', [Validators.required, Validators.minLength(MIN_PASSWORD_LENGTH)]] });
    readonly registerForm = this.m_formBuilder.nonNullable.group({ username:        ['', [Validators.required, Validators.minLength(MIN_NAME_LENGTH)]],
                                                                   email:           ['', [Validators.required, Validators.email]],
                                                                   password:        ['', [Validators.required, Validators.minLength(MIN_PASSWORD_LENGTH)]],
                                                                   confirmPassword: ['', [Validators.required]],
                                                                   first_name:      ['', [Validators.required, Validators.minLength(MIN_NAME_LENGTH)]],
                                                                   last_name:       ['', [Validators.required, Validators.minLength(MIN_NAME_LENGTH)]] },
                                                                 { validators: passwordsMustMatch });

    constructor(private themeService: ThemeService) {}

    public get hideConfirmPassword()  { return this.ui.hideConfirmPassword; }
    public get hideLoginPassword()    { return this.ui.hideLoginPassword; }
    public get hideRegisterPassword() { return this.ui.hideRegisterPassword; }
    public get incorrectCredentials() { return this.ui.incorrectCredentials; }
    public get mode()                 { return this.ui.mode; }
    public get nodes() { return Array.from({ length: 35 }, (_, i) => i + 1); }

    public get submitting()           { return this.ui.submitting; }
    public get theme()                { return this.themeService.theme; }
    public get minNameLength()      { return MIN_NAME_LENGTH; }
    public get minPasswordLength()  { return MIN_PASSWORD_LENGTH; }

    public currentForm() {
        return this.isLogin() ? this.loginForm : this.registerForm;
    }

    private failedLogin(error: HttpErrorResponse) {
        if (error.status !== 404)
            throw new Error(error.message);

        this.showIncorrectCredentials();
    }

    private isLogin() : boolean {
        return this.ui.mode() == "login";
    }

    private showIncorrectCredentials() {
        if (this.ui.incorrectCredentials())
            return;

        this.ui.incorrectCredentials.set(true);
        setTimeout(() => { this.ui.incorrectCredentials.set(false); }, 5 * SECOND_IN_MS);
    }

    public submit(): void {
        const form = this.currentForm();
        if (form.invalid) {
            if (form.dirty && this.isLogin())
                this.showIncorrectCredentials()
            form.markAllAsTouched();
            return;
        }

        this.ui.submitting.set(true);
        try {
            if (this.isLogin())
                this.m_userService.login(this.loginForm.getRawValue(), this.failedLogin.bind(this));
            else
                console.log('Register submitted', this.registerForm.getRawValue());
        } catch (error) {
            form.reset();
            form.markAllAsTouched();
        }
        this.ui.submitting.set(false);
    }

    public switchMode(next: Mode): void {
        this.ui.mode.set(next);
    }

    public toggleConfirmPasswordVisibility(event: Event): void {
        event.preventDefault();
        this.ui.hideConfirmPassword.update((v) => !v);
    }

    public toggleLoginPasswordVisibility(event: Event): void {
        event.preventDefault();
        this.ui.hideLoginPassword.update((v) => !v);
    }

    public toggleRegisterPasswordVisibility(event: Event): void {
        event.preventDefault();
        this.ui.hideRegisterPassword.update((v) => !v);
    }
}